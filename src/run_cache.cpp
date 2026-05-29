#include "run_cache.h"

void run_cache(EventQueue& queue, MemoryManager& manager) {
    std::cout << "[+] Cache Pipeline Started" << std::endl;
    
    while (true) {
        // 1. Ask permission based on Type ID BEFORE popping from network queue
        manager.is_allowed_to_pop(EventID::KLINE);

        // 2. Safely pop knowing the memory matrix isn't choked
        auto event = queue.wait_and_pop();
        
        // 3. Process ingestion routing
        manager.wait_and_push(std::move(event));
    }
}