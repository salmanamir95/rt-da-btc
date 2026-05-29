#include "cache/run_cache.h"

void run_cache(EventQueue& queue, MemoryManager& manager) {
    
    while (true) {
        manager.is_allowed_to_pop(EventID::KLINE);


        auto event = queue.wait_and_pop();
        
        manager.wait_and_push(std::move(event));
    }
}