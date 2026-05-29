#include "B0.h"
#include <iostream>

B0::B0(const nlohmann::json& payload) : EventMeta(EventID::BO) {
    if (payload.contains("E")) {
        timestamp = payload["E"].get<uint64_t>();
    } else {
        timestamp = 0;
    }
    if (payload.contains("s")) {
        symbol = payload["s"].get<std::string>();
    }
    if (payload.contains("e")) {
        eventType = payload["e"].get<std::string>();
    }
}

void B0::print() const {
    std::cout << "B0 Event: " << symbol 
              << " | Timestamp: " << timestamp 
              << " | Event Type: " << eventType << std::endl;
}