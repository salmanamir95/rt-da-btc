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

    void is_allowed_to_pop(uint8_t event_id) {
        // If the lane is currently locked in State 1 (Analytics running), 
        // we freeze here BEFORE popping a new item from the EventQueue
        while (lane_states[event_id].load(std::memory_order_acquire) == 1) {
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
            #endif
        }
    }

    void is_allowed_to_push(uint8_t event_id) {
        // Hit the Dispatch Gate (State 2)
        lane_states[event_id].store(2, std::memory_order_release);

        // Wait until Analytics gives permission (State 0)
        while (lane_states[event_id].load(std::memory_order_acquire) != 0) {
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
            #endif
        }
    }

    void signal_push_complete(uint8_t event_id) {
        // Lock the data lane instantly for Analytics (State 1)
        lane_states[event_id].store(1, std::memory_order_release);
    }

    // --- ANALYTICS ENGINE GATES ---

    void is_allowed_analytics(uint8_t event_id) {
        // Block the analytics pipeline until the lane turns to State 1
        while (lane_states[event_id].load(std::memory_order_acquire) != 1) {
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
            #endif
        }
    }

    void give_permission_to_proceed(uint8_t event_id) {
        // Wait until cache thread is actually waiting at the dispatch gate (State 2)
        // (Bypassed naturally during the initial warmup phase up to size 8)
        if (event_id == EventID::KLINE && kline_window.is_full()) {
            while (lane_states[event_id].load(std::memory_order_acquire) != 2) {
                #if defined(__x86_64__) || defined(_M_X64)
                __builtin_ia32_pause();
                #endif
            }
        }
        // Release the lane back to State 0
        lane_states[event_id].store(0, std::memory_order_release);
    }

    // --- CORE INGESTION ROUTER ---

    void wait_and_push(std::unique_ptr<EventMeta> event) {
        if (!event) return;

        switch (event->type_id) {
            case EventID::KLINE: {
                std::unique_ptr<Kline> kline_ptr(static_cast<Kline*>(event.release()));
                
                if (kline_window.is_full()) {
                    is_allowed_to_push(EventID::KLINE);
                    kline_window.shift_and_insert(std::move(kline_ptr));
                    signal_push_complete(EventID::KLINE);
                } else {
                    kline_window.warmup_insert(std::move(kline_ptr));
                    if (kline_window.is_full()) {
                        signal_push_complete(EventID::KLINE);
                    }
                }
                break;
            }
            // case EventID::BO: ...
        }
    }

    const Window<Kline>& get_kline_window() const { return kline_window; }
    Window<Kline>& get_kline_window() { return kline_window; }
};