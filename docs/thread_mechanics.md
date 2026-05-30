# RT-DA: Deep Dive - Thread Mechanics & Concurrency

This document breaks down the specific concurrency models, thread lifecycles, and class topologies for the three distinct domains of the RT-DA pipeline: **Ingestor**, **Cache**, and **Analytics**.

---

## 1. The Ingestor Domain (Network & Queueing)

The Ingestor domain is responsible for asynchronous network I/O, JSON parsing, and safely crossing the boundary from the network to the internal application queue.

### Class Diagram
```mermaid
classDiagram
    class WebSocketEngine {
        +ix::WebSocket webSocket
        +start()
        +setOnMessageCallback()
    }
    class BinanceFactory {
        <<static>>
        +create(json payload)$ unique_ptr~EventMeta~
    }
    class EventQueue {
        -queue~unique_ptr~EventMeta~~ q
        -mutex mtx
        -condition_variable cv
        +push(unique_ptr~EventMeta~)
        +wait_and_pop() unique_ptr~EventMeta~
    }
    class EventMeta {
        <<Polymorphic Base>>
    }

    WebSocketEngine --> BinanceFactory : delegates parsing
    BinanceFactory --> EventMeta : instantiates
    WebSocketEngine --> EventQueue : thread-safe push
```

### Concurrency Sequence (How the Threads Work)
The `ixwebsocket` library runs its own internal background thread to listen to the TCP socket. When a packet arrives, it fires a callback into our application space, which we parse and push into a mutex-locked queue.

```mermaid
sequenceDiagram
    participant OS as OS / Network TCP
    participant IX as ix::WebSocket Background Thread
    participant Factory as BinanceFactory
    participant Q as EventQueue (Mutex + CV)

    OS->>IX: Raw Byte Stream Arrival
    activate IX
    IX->>IX: Assemble WebSocket Frame
    IX->>Factory: Callback with JSON string
    activate Factory
    Factory-->>IX: Returns unique_ptr<Kline>
    deactivate Factory
    
    IX->>Q: push(unique_ptr<Kline>)
    activate Q
    Q->>Q: lock std::mutex
    Q->>Q: push to std::queue
    Q->>Q: cv.notify_one()
    Q->>Q: unlock std::mutex
    deactivate Q
    deactivate IX
```

---

## 2. The Cache Domain (Memory Router)

The Cache thread acts as the ultimate decoupled relayer. It bridges the slow, blocking network queue with the ultra-fast, lock-free analytics matrix.

### Class Diagram
```mermaid
classDiagram
    class RunCache {
        +main_loop()
    }
    class EventQueue {
        +wait_and_pop() unique_ptr~EventMeta~
    }
    class MemoryManager {
        -atomic~int~ lane_states
        +wait_and_push(unique_ptr~EventMeta~)
        -is_allowed_to_push()
        -signal_push_complete()
    }
    class Window~Kline~ {
        +shift_and_insert()
    }

    RunCache --> EventQueue : pops from
    RunCache --> MemoryManager : routes to
    MemoryManager --> Window~Kline~ : manages memory
```

### Concurrency Sequence (How the Threads Work)
The Cache thread loops infinitely. It sleeps with 0% CPU usage while waiting for network packets. Once a packet arrives, it dynamically routes it. If the memory matrix is full, it switches to spin-lock atomics to negotiate safe memory access with the Analytics Engine.

```mermaid
sequenceDiagram
    participant Q as EventQueue (CV)
    participant Cache as Cache Thread
    participant State as Atomic State (lane_states)
    participant Win as Window Matrix

    loop Infinite Loop
        Cache->>Q: wait_and_pop()
        Note over Q: Cache Thread Sleeps (0% CPU)<br/>waiting for Network
        Q-->>Cache: Returns Event
        
        Cache->>Cache: Route by type_id (e.g. KLINE)
        
        Cache->>State: is_allowed_to_push()
        Note over State: State = 2 (Stands at Gate)
        Cache->>Cache: Spinlock (__builtin_ia32_pause) until State == 0
        
        Note over Cache: Analytics sets State to 0
        Cache->>Win: shift_and_insert() (Safe Memory Write)
        Cache->>State: signal_push_complete() (Sets State = 1)
    end
```

---

## 3. The Analytics Domain (Math Engine & Fan-Out)

The Analytics domain is the heaviest CPU consumer. It is designed to sit idle, wake up upon a memory update, and aggressively fan out its mathematical operations across multiple sub-threads to maximize hardware utilization.

### Class Diagram
```mermaid
classDiagram
    class IngressAnalytics {
        -MemoryManager& memory
        +run_pipeline_kline()
    }
    class PipelineKline {
        <<static>>
        +runPipeline(Window~Kline~)
    }
    class AnalyticsFactories {
        <<static>>
        +MeanFactoryKline, ModeFactoryKline, MedianFactoryKline, StdDevFactoryKline
    }
    class Threads {
        <<std::thread>>
        +tOP, tCP, tHP, tLP, tVol (x4)
    }
    class MetricsResults {
        <<struct>>
        +MeanResult, ModeResult, MedianResult, StdDevResult
    }

    IngressAnalytics --> PipelineKline : triggers
    PipelineKline --> AnalyticsFactories : spawns 4 threads for
    AnalyticsFactories --> Threads : spawns 20 sub-threads (5 each)
    Threads --> MetricsResults : populate fields
```

### Concurrency Sequence (How the Threads Work)
This is where the massive multi-threading takes place. The main Analytics thread waits for State 1. Once triggered, it launches 4 dedicated Factory Threads (Mean, Mode, Median, StdDev). Each of these factories then launches 5 Math Sub-Threads for parallel calculations (OP, CP, HP, LP, Vol). All 20 threads compute in parallel, join back together, and return their respective metric structs.

```mermaid
sequenceDiagram
    participant Ana as Analytics Main Thread
    participant State as Atomic State
    participant Pipe as PipelineKline
    participant Fact as 4x Factory Threads (Mean, Mode, Median, StdDev)
    participant Sub as 20x Math Sub-Threads (tOP...tVol x 4)

    loop Infinite Loop
        Ana->>State: is_allowed_analytics()
        Note over Ana: Spins until State == 1
        
        Ana->>Pipe: runPipeline(window)
        activate Pipe
        
        Pipe->>Fact: Launch 4x std::thread (MeanFactory, ModeFactory, etc.)
        activate Fact
        
        Fact->>Sub: Spawns 20x std::thread (calcMeanOP, calcModeOP, etc.)
        activate Sub
        Note over Sub: True Parallel Execution<br/>across multiple CPU cores (24 total active threads)
        
        Sub-->>Fact: join() 20 threads
        deactivate Sub
        
        Fact-->>Pipe: returns Populated Metric Results
        deactivate Fact
        
        Pipe-->>Ana: All 4 Factory Threads join()
        deactivate Pipe
        
        Ana->>State: give_permission_to_proceed()
        Note over Ana: Spins until State == 2 (Cache arrives)
        Ana->>State: Sets State = 0 (Releases Lock)
    end
```
