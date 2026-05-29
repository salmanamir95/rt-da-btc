#pragma once

#include "cache/memory/window.h"
#include "kline.h" 
#include "EventMeta.h"
#include <memory>
#include <iostream>

class MemoryManager {
private:
    Window<Kline> kline_window;

public:
    MemoryManager() = default;

    void wait_and_push(std::unique_ptr<EventMeta> event) {
        if (!event) return;

        switch (event->type_id) {
            case EventID::KLINE: {
                std::unique_ptr<Kline> kline_ptr(static_cast<Kline*>(event.release()));
                kline_window.wait_and_push(std::move(kline_ptr));
                break;
            }
            case EventID::BO:
                break;
            default:
                std::cerr << "[-] MemoryManager Error: Unhandled Event ID" << std::endl;
                break;
        }
    }

    // Pass-through Handshake Proxies for Analytics
    void wait_for_analytics_permission() { kline_window.wait_for_analytics_permission(); }
    void give_permission_to_proceed() { kline_window.give_permission_to_proceed(); }

    const Window<Kline>& get_kline_window() const { return kline_window; }
    Window<Kline>& get_kline_window() { return kline_window; }
};