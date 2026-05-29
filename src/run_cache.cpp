#include "run_cache.h"

void run_cache(EventQueue& queue, MemoryManager& manager) {
    std::cout << "[+] Cache Pipeline Started" << std::endl;
    
    while (true) {
        std::cout<< "[Cache] Ask permission based on Type ID BEFORE popping from network queue" <<std::endl;
        manager.is_allowed_to_pop(EventID::KLINE);

        std::cout<< "[Cache] Safely pop knowing the memory matrix isn't choked" <<std::endl;
        
        auto event = queue.wait_and_pop();
        
        std::cout<< "[Cache] Process ingestion routing" <<std::endl;
        
        manager.wait_and_push(std::move(event));
    }
}