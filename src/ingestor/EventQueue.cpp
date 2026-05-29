#include "ingestor/EventQueue.h"

void EventQueue::push(std::unique_ptr<EventMeta> event) {
    if (!event) return;
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(std::move(event));
    }
    cv.notify_one(); 
}

std::unique_ptr<EventMeta> EventQueue::wait_and_pop() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return !queue.empty(); });
    std::unique_ptr<EventMeta> event = std::move(queue.front());
    queue.pop();
    return event;
}

std::unique_ptr<EventMeta> EventQueue::try_pop() {
    std::lock_guard<std::mutex> lock(mtx);
    if (queue.empty()) {
        return nullptr;
    }
    std::unique_ptr<EventMeta> event = std::move(queue.front());
    queue.pop();
    return event;
}

bool EventQueue::empty() {
    std::lock_guard<std::mutex> lock(mtx);
    return queue.empty();
}

size_t EventQueue::size() {
    std::lock_guard<std::mutex> lock(mtx);
    return queue.size();
}
