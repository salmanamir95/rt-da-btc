#pragma once    


#include <memory>
#include <nlohmann/json.hpp>
#include "EventMeta.h"
#include "kline.h"
#include "B0.h"

class BinanceFactory {
public:
    static std::unique_ptr<EventMeta> createEvent(const nlohmann::json& payload) {
        std::string eventType = payload["e"].get<std::string>();

        if (eventType == "kline") {
            // Pass the whole JSON tree straight into the Kline constructor
            return std::make_unique<Kline>(payload);
        } else {
            // Fallback for everything else
            return std::make_unique<B0>(payload);
        }
    }
};