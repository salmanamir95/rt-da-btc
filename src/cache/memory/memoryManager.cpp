#include "cache/memory/memoryManager.h"

void MemoryManager::is_allowed_to_pop(uint8_t event_id) {
    while (lane_states[event_id].load(std::memory_order_acquire) == 1) {
        #if defined(__x86_64__) || defined(_M_X64)
        __builtin_ia32_pause();
        #endif
    }
}

void MemoryManager::is_allowed_to_push(uint8_t event_id) {
    lane_states[event_id].store(2, std::memory_order_release);
    while (lane_states[event_id].load(std::memory_order_acquire) != 0) {
        #if defined(__x86_64__) || defined(_M_X64)
        __builtin_ia32_pause();
        #endif
    }
}

void MemoryManager::signal_push_complete(uint8_t event_id) {
    lane_states[event_id].store(1, std::memory_order_release);
}

void MemoryManager::is_allowed_analytics(uint8_t event_id) {
    while (lane_states[event_id].load(std::memory_order_acquire) != 1) {
        #if defined(__x86_64__) || defined(_M_X64)
        __builtin_ia32_pause();
        #endif
    }
}

void MemoryManager::give_permission_to_proceed(uint8_t event_id) {
    if (event_id == EventID::KLINE && kline_window.is_full()) {
        while (lane_states[event_id].load(std::memory_order_acquire) != 2) {
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
            #endif
        }
    }
    lane_states[event_id].store(0, std::memory_order_release);
}

void MemoryManager::wait_and_push(std::unique_ptr<EventMeta> event) {
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
    }
}
