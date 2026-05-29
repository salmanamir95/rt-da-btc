#pragma once

#include "cache/memory/window.h"
#include "kline.h" 
#include "EventMeta.h"
#include <memory>
#include <atomic>
#include <iostream>

class MemoryManager {
private:
    Window<Kline> kline_window;
    // Window<OrderBook> bo_window; // Ready for the future lane

    // Independent handshake state trackers for each EventID lane
    // 0 = Ingestion/Pop Turn, 1 = Analytics Turn, 2 = Push/Dispatch Gate
    std::atomic<int> lane_states[EventID::MAX_LANES]{ {0}, {0} };

public:
    MemoryManager() = default;

    // --- CACHE THREAD GATES ---

    void is_allowed_to_pop(uint8_t event_id);
    void is_allowed_to_push(uint8_t event_id);
    void signal_push_complete(uint8_t event_id);

    // --- ANALYTICS ENGINE GATES ---
    void is_allowed_analytics(uint8_t event_id);
    void give_permission_to_proceed(uint8_t event_id);

    // --- CORE INGESTION ROUTER ---
    void wait_and_push(std::unique_ptr<EventMeta> event);

    const Window<Kline>& get_kline_window() const { return kline_window; }
    Window<Kline>& get_kline_window() { return kline_window; }
};