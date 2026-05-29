#include "run_cache.h"

void run_cache(EventQueue& queue, MemoryManager& manager)
{
    std::cout << "[+] Cache Pipeline Started" << std::endl;
    
    while (true)
    {
        // 1. Grabs next real-time network packet
        auto event = queue.wait_and_pop();
        
        // 2. Handshake handling is fully encapsulated inside here!
        // Automatically asks for permission, shifts when allowed, and updates state.
        manager.wait_and_push(std::move(event));
    }
}