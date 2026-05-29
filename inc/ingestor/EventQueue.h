#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

#include "ingestor/EventMeta.h"

class EventQueue {
private:
    // Enforcing strict single-instance ownership inside the underlying container
    std::queue<std::unique_ptr<EventMeta>> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    // Takes full, exclusive ownership of the event and pushes it into the queue
    void push(std::unique_ptr<EventMeta> event);
    std::unique_ptr<EventMeta> wait_and_pop();
    std::unique_ptr<EventMeta> try_pop();
    bool empty();
    size_t size();
};