#pragma once    


#include <memory>
#include <nlohmann/json.hpp>
#include "ingestor/EventMeta.h"
#include "ingestor/kline.h"
#include "ingestor/B0.h"

class BinanceFactory {
public:
    static std::unique_ptr<EventMeta> createEvent(const nlohmann::json& payload);
};