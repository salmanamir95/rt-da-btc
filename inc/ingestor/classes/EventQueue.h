#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

#include "EventMeta.h"

class EventQueue {
private:
    // Enforcing strict single-instance ownership inside the underlying container
    std::queue<std::unique_ptr<EventMeta>> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    // Takes full, exclusive ownership of the event and pushes it into the queue
    void push(std::unique_ptr<EventMeta> event) {
        if (!event) return; // Guard against null pointers
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(std::move(event)); // std::move is mandatory here
        }
        cv.notify_one(); 
    }

    // Blocks the execution thread until an event arrives, then returns single ownership
    std::unique_ptr<EventMeta> wait_and_pop() {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Put the thread to sleep efficiently if nothing is in the queue
        cv.wait(lock, [this]() { return !queue.empty(); });

        // Retrieve ownership from the front of the queue
        std::unique_ptr<EventMeta> event = std::move(queue.front());
        queue.pop();
        
        return event; // RVO (Return Value Optimization) handles this smoothly
    }

    // Non-blocking alternative check
    std::unique_ptr<EventMeta> try_pop() {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) {
            return nullptr;
        }
        
        std::unique_ptr<EventMeta> event = std::move(queue.front());
        queue.pop();
        return event;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }
};