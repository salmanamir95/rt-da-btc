// Kline.cpp
#include "kline.h"
#include <iostream>

Kline::Kline(const nlohmann::json& payload) : EventMeta(EventID::KLINE) {
    // 1. Hydrate the Parent variables (EventMeta)
    this->eventType = payload["e"].get<std::string>();
    this->timestamp = payload["E"].get<uint64_t>();
    this->symbol    = payload["s"].get<std::string>();

    // 2. Hydrate the local variables directly from the inner "k" object
    const auto& k = payload["k"];
    this->startTime = k["t"].get<uint64_t>();
    this->closeTime = k["T"].get<uint64_t>();
    this->interval  = k["i"].get<std::string>();
    this->isClosed  = k["x"].get<bool>();

    // Convert string prices/volumes to fast doubles for calculations
    this->openPrice  = std::stod(k["o"].get<std::string>());
    this->closePrice = std::stod(k["c"].get<std::string>());
    this->highPrice  = std::stod(k["h"].get<std::string>());
    this->lowPrice   = std::stod(k["l"].get<std::string>());
    this->volume     = std::stod(k["v"].get<std::string>());
}

void Kline::process() {
    // The data is baked into this object instance. 
    // Now you pass it along to your engine or run calculations!
    std::cout << "[" << symbol << "] Current streaming price: " << closePrice;
    
    if (isClosed) {
        std::cout << " -> Candle Closed! Sending to Indicators pipeline.";
        // Run indicators, trigger alerts, etc.
    } else {
        std::cout << " (Live updating...)";
    }
    std::cout << std::endl;
}

void Kline::print() const {
    std::cout << "Kline Event: " << symbol 
              << " | Open: " << openPrice 
              << " | High: " << highPrice 
              << " | Low: " << lowPrice 
              << " | Close: " << closePrice 
              << " | Vol: " << volume 
              << " | Closed: " << (isClosed ? "YES" : "NO") 
              << " | Timestamp: " << timestamp 
              << " | Event Type: " << eventType 
              << " | Start Time: " << startTime 
              << " | End Time: " << closeTime 
              << " | Interval: " << interval 
              << std::endl;
}   