#pragma once

#include "ingestor/EventMeta.h"
#include <array>
#include <memory>
#include <utility>

template <typename T>
class Window {
private:
    static_assert(std::is_base_of_v<EventMeta, T>, "T must derive from EventMeta");

    std::array<std::unique_ptr<T>, 8> window;
    size_t current_size = 0;

public:
    Window() = default;

    void warmup_insert(std::unique_ptr<T> event) {
        if (current_size < 8) {
            window[current_size] = std::move(event);
            current_size++;
        }
    }

    void shift_and_insert(std::unique_ptr<T> event) {
        for (size_t i = 1; i < 8; ++i) {
            window[i - 1] = std::move(window[i]);
        }
        window[7] = std::move(event);
    }

    // Chronological Accessors
    const T* c0() const { return window[0].get(); } 
    const T* c1() const { return window[1].get(); } 
    const T* c2() const { return window[2].get(); } 
    const T* c3() const { return window[3].get(); } 
    const T* c4() const { return window[4].get(); } 
    const T* c5() const { return window[5].get(); } 
    const T* c6() const { return window[6].get(); } 
    const T* c7() const { return window[7].get(); } 
    size_t size() const { return current_size; }
    bool is_full() const { return current_size == 8; }
};