#include "run_ingestor.h"
#include "EventQueue.h"
#include "run_cache.h"
#include <thread>

int main() {
    EventQueue queue;
    MemoryManager mem;
    std::string url = "wss://fstream.binance.com/market/ws/btcusdt@kline_1m";
    std::thread ingestorThread(run_ingestor, url, std::ref(queue));
    std::thread cacheThread(run_cache, std::ref(queue), std::ref(mem));
    if (ingestorThread.joinable()) {
        ingestorThread.join(); 
    }
    if (cacheThread.joinable()) {
        cacheThread.join();
    }

    return 0;
}
