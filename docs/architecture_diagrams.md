# RT-DA: Master Architecture & Diagrams

## 📋 The Master Diagram List

| Category | Diagram Type | Targeted Portfolio Purpose |
| --- | --- | --- |
| **Concurrency & Handshake** | 1. Concurrency Swimlane / Sequence Diagram | Tracks timeline/coordination across the 3 main threads and sub-workers. |
|  | 2. 3-State Finite State Machine (FSM) | Maps the exact $0 \rightarrow 1 \rightarrow 2 \rightarrow 0$ lock-free carousel transitions. |
| **Architectural Topology** | 3. UML Class Diagram | Displays inheritance (`EventMeta`), encapsulation, and composition relationships. |
|  | 4. Entity-Relationship Diagram (ERD) | Maps the physical data schema definitions (Klines, Order Books, Metrics). |
|  | 5. Component / Ball-and-Socket Diagram | Shows structural dependencies and clean interface boundaries. |
| **Data Flow (DFD)** | 6. DFD Level-0: Context Diagram | Illustrates how the engine treats external WebSocket inputs and console outputs. |
|  | 7. DFD Level-1: Processing Pipeline | Tracks memory transformations from raw JSON bytes down to `Window` memory. |
|  | 8. DFD Level-2: Multi-Lane Routing | Shows the isolated parallel execution lanes inside the `MemoryManager`. |
| **Hardware & Performance** | 9. Memory Layout & Cache Alignment | Displays pointer displacement mechanics (`std::move`) in cache lines. |

---

## 🏛️ Comprehensive Structural Breakdowns

### 1. UML Class Diagram

This captures the object-oriented structure of your C++ engine. It visually proves your separation of concerns and type safety to architects reading your portfolio.

```mermaid
classDiagram
    class EventMeta {
        <<abstract>>
        +uint8_t type_id
        +print() void*
    }

    class Kline {
        +uint64_t startTime
        +uint64_t closeTime
        +string interval
        +double openPrice
        +double closePrice
        +double highPrice
        +double lowPrice
        +double volume
        +bool isClosed
        +Kline(json payload)
        +print() void
    }

    class B0 {
        +B0(json payload)
        +print() void
    }

    EventMeta <|-- Kline
    EventMeta <|-- B0

    class Window~T~ {
        -array~unique_ptr~T~~, 8~ window
        -size_t current_size
        +warmup_insert(unique_ptr~T~ event)
        +shift_and_insert(unique_ptr~T~ event)
        +is_full() bool
    }

    class MemoryManager {
        -atomic~int~ lane_states[MAX_LANES]
        +wait_and_push(unique_ptr~EventMeta~ event)
        +is_allowed_to_pop(uint8_t event_id)
        +is_allowed_to_push(uint8_t event_id)
        +signal_push_complete(uint8_t event_id)
        +is_allowed_analytics(uint8_t event_id)
        +give_permission_to_proceed(uint8_t event_id)
    }

    MemoryManager *-- "1" Window~Kline~ : owns

    class IngressAnalytics {
        -MemoryManager& memory
        +IngressAnalytics(MemoryManager& memory)
        +run_pipeline_kline()
    }
    
    IngressAnalytics o-- MemoryManager : references

    class PipelineKline {
        +MeanFactory(Window~Kline~& window)$ MeanResult
        +ModeFactory(Window~Kline~& window)$ ModeResult
        +MedianFactory(Window~Kline~& window)$ MedianResult
        +StdDevFactory(Window~Kline~& window)$ StdDevResult
        +runPipeline(Window~Kline~& window)$ void
    }
    
    class MeanFactoryKline {
        +compute(Window~Kline~& window)$ MeanResult
    }
    class ModeFactoryKline {
        +compute(Window~Kline~& window)$ ModeResult
    }
    class MedianFactoryKline {
        +compute(Window~Kline~& window)$ MedianResult
    }
    class StdDevFactoryKline {
        +compute(Window~Kline~& window)$ StdDevResult
    }

    PipelineKline ..> MeanFactoryKline : utilizes
    PipelineKline ..> ModeFactoryKline : utilizes
    PipelineKline ..> MedianFactoryKline : utilizes
    PipelineKline ..> StdDevFactoryKline : utilizes
```

### 2. Entity-Relationship Diagram (ERD)

Even though you are using high-speed in-memory structures rather than a standard relational SQL database, an ERD is vital to define your data models.

```mermaid
erDiagram
    Kline {
        uint64_t startTime
        uint64_t closeTime
        string interval
        double openPrice
        double closePrice
        double highPrice
        double lowPrice
        double volume
        bool isClosed
    }
    
    MeanResult {
        uint64_t time
        double MeanOP
        double MeanCP
        double MeanHP
        double MeanLP
        double MeanVol
    }
    
    ModeResult {
        uint64_t time
        double ModeOP
        double ModeCP
        double ModeHP
        double ModeLP
        double ModeVol
    }
    
    MedianResult {
        uint64_t time
        double MedianOP
        double MedianCP
        double MedianHP
        double MedianLP
        double MedianVol
    }
    
    StdDevResult {
        uint64_t time
        double StdDevOP
        double StdDevCP
        double StdDevHP
        double StdDevLP
        double StdDevVol
    }
    
    B0 {
        string eventType
        uint64_t timestamp
    }

    Kline ||--o{ MeanResult : "1 Window generates 1 MeanResult"
    Kline ||--o{ ModeResult : "1 Window generates 1 ModeResult"
    Kline ||--o{ MedianResult : "1 Window generates 1 MedianResult"
    Kline ||--o{ StdDevResult : "1 Window generates 1 StdDevResult"
```

### 3. Component Diagram (Ball-and-Socket)

This presents your modular software engineering patterns.

```mermaid
flowchart LR
    subgraph Ingestion["Websocket Ingestor Component"]
        IX[ixwebsocket]
        Factory[BinanceFactory]
    end

    subgraph Cache["Memory Cache Component"]
        Manager[MemoryManager]
        Win[Window Matrix]
    end

    subgraph Analytics["Analytics Engine Component"]
        Pipe[IngressAnalytics]
        Math[PipelineKline]
    end

    Ingestion -- "std::unique_ptr<EventMeta>" --> Queue[(EventQueue)]
    Queue -- "Pops Event" --> Cache
    
    %% Ball and socket representation via API boundary
    Cache -- "Wait for Permission API" --> Analytics
    Analytics -- "State 0/1/2 Handshake" --> Cache
```

### 4. Concurrency Swimlane / Sequence Diagram

This maps execution across horizontal hardware layout lanes, tracking the $0 \rightarrow 1 \rightarrow 2 \rightarrow 0$ synchronization over time.

```mermaid
sequenceDiagram
    participant Net as Network Thread
    participant Q as EventQueue
    participant Cache as Cache Thread
    participant State as lane_states[KLINE]
    participant Ana as Analytics Thread
    participant Threads as 20x Math Threads

    Note over State: State = 0 (Cache Turn)
    Net->>Q: push(Kline)
    Cache->>Q: wait_and_pop()
    Q-->>Cache: returns Kline
    Cache->>State: is_allowed_to_push() [Checks if Analytics is busy]
    Cache->>Cache: shift_and_insert()
    Cache->>State: signal_push_complete() -> Sets State = 1
    
    Note over State: State = 1 (Analytics Turn)
    State-->>Ana: Wakes up (is_allowed_analytics)
    Ana->>Threads: Spawns 4x Factories, launching 20x Math Threads
    
    Note right of Net: Cache is NOT blocked.<br/>It waits for the next network packet.
    Net->>Q: push(next Kline)
    Cache->>Q: wait_and_pop()
    Q-->>Cache: returns next Kline
    
    Cache->>State: is_allowed_to_push() -> Sets State = 2 (Gate)
    Note over State: State = 2 (Cache waiting at Gate)
    
    Threads-->>Ana: All 20 threads and 4 factories join()
    Ana->>State: give_permission_to_proceed() -> Sets State = 0
    Note over State: State = 0 (Cycle Repeats)
    State-->>Cache: Wakes up Cache
```

### 5. DFD Levels (Context down to Multi-Lane Execution)

**Level 0: Context Diagram**
```mermaid
flowchart LR
    Binance((Binance JSON Stream))
    Core[RT-DA Core Engine]
    Out((Computed Metric Vector Packets))

    Binance -->|Websocket| Core
    Core -->|Console / DB| Out
```

**Level 1: Processing Pipeline**
```mermaid
flowchart LR
    Raw[Raw JSON] --> Hydrate[Parsing/Hydration]
    Hydrate -->|EventMeta| Q[Queue Ingestion]
    Q --> Route[Memory Routing]
    Route -->|Window<Kline>| Calc[Matrix Calculations]
    Calc --> MeanResult
    Calc --> ModeResult
    Calc --> MedianResult
    Calc --> StdDevResult
```

**Level 2: Multi-Lane Routing**
```mermaid
flowchart TD
    Router[MemoryManager router]
    
    Router -->|if type_id == KLINE| Lane0[Lane 0: KLINE]
    Router -->|if type_id == B0| Lane1[Lane 1: BO]
    
    subgraph KLINE [KLINE Pipeline]
        State0((State Tracker 0))
        Win0[Window<Kline>]
        Math0[PipelineKline]
    end
    
    subgraph B0_LANE [OrderBook Pipeline]
        State1((State Tracker 1))
        Win1[Window<OrderBook>]
        Math1[PipelineBO]
    end

    Lane0 --> Win0
    Win0 <--> State0
    State0 <--> Math0
```

### 6. Memory Layout & Cache Alignment Diagram

Demonstrates systems programming awareness and CPU Cache Line efficiency.

```mermaid
block-beta
    columns 8
    block:header:8
        Title["std::array (std::unique_ptr(Kline), 8)"]
    end
    
    %% Array addresses
    Ptr0["[0]"] Ptr1["[1]"] Ptr2["[2]"] Ptr3["[3]"] Ptr4["[4]"] Ptr5["[5]"] Ptr6["[6]"] Ptr7["[7]"]
```
> **Systems Optimization Sidebar:**
> When `shift_and_insert()` is called, the CPU is NOT copying heavy 80+ byte `Kline` data structs around memory. It is purely performing `std::move` on 8-byte heap pointer addresses (`std::unique_ptr`). Because the contiguous `std::array` of 8 pointers spans exactly 64 bytes, it fits perfectly inside a single **L1/L2 CPU Cache Line**. This ensures hardware-level memory bank fetches are kept to an absolute minimum, bypassing expensive RAM lookups.
