#include "ingestor/BinanceFactory.h"
#include "ingestor/kline.h"
#include "ingestor/B0.h"

std::unique_ptr<EventMeta> BinanceFactory::createEvent(const nlohmann::json& payload) {
    std::string eventType = payload["e"].get<std::string>();

    if (eventType == "kline") {
        return std::make_unique<Kline>(payload);
    } else {
        return std::make_unique<B0>(payload);
    }
}
