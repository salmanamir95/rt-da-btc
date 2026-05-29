#pragma once

#include "EventMeta.h"
#include <array>
#include <memory>
#include <utility>
#include <type_traits> 
#include <atomic> 

template <typename T>
class Window {
private:
    static_assert(std::is_base_of_v<EventMeta, T>, "CRITICAL: T must derive from EventMeta");

    std::array<std::unique_ptr<T>, 8> window;
    size_t current_size = 0;
    
    // Strict Step-By-Step Coordination States:
    // 0 = Ingestion Turn (Math Thread has permission to work)
    // 1 = Analysis Turn (Analytics Engine has permission to work)
    // 2 = Dispatch Gate (Math Thread has loaded next event, waiting for permission to proceed)
    std::atomic<int> handshake_state{0}; 

public:
    Window() = default;

    /**
     * Called by Math Thread inside manager.wait_and_push()
     */
    void wait_and_push(std::unique_ptr<T> event) {
        if (current_size == 8) {
            // 1. Hit the Dispatch Gate and signal we are ready to roll
            handshake_state.store(2, std::memory_order_release);

            // 2. Ask for permission to proceed (wait here until Analytics signals state 0)
            while (handshake_state.load(std::memory_order_acquire) != 0) {
                #if defined(__x86_64__) || defined(_M_X64)
                __builtin_ia32_pause();
                #endif
            }

            // 3. Permission granted: Shift the displacement matrix
            for (size_t i = 1; i < 8; ++i) {
                window[i - 1] = std::move(window[i]);
            }
            window[7] = std::move(event);

            // 4. Instantly hand permission back to the Analytics Engine (State 1)
            handshake_state.store(1, std::memory_order_release);

        } else {
            // Buffer warmup phase
            window[current_size] = std::move(event);
            current_size++;
            
            // Hand the very first full batch straight to analytics
            if (current_size == 8) {
                handshake_state.store(1, std::memory_order_release);
            }
        }
    }

    /**
     * Handshake Methods for the Analytics Engine
     */
    void wait_for_analytics_permission() {
        while (handshake_state.load(std::memory_order_acquire) != 1) {
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
            #endif
        }
    }

    void give_permission_to_proceed() {
        // Enforce synchronization: Wait until math thread is actually waiting at the gate
        while (handshake_state.load(std::memory_order_acquire) != 2) {
            #if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
            #endif
        }
        // Release the math thread
        handshake_state.store(0, std::memory_order_release);
    }

    // Chronological Accessors
    const T* c0() const { return window[0].get(); } 
    const T* c7() const { return window[7].get(); } 
    size_t size() const { return current_size; }
    bool is_full() const { return current_size == 8; }
};