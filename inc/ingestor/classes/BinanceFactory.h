#pragma once    


#include <memory>
#include <nlohmann/json.hpp>
#include "EventMeta.h"
#include "kline.h"
#include "B0.h"

class BinanceFactory {
public:
    static std::unique_ptr<EventMeta> createEvent(const nlohmann::json& payload);
};