// Kline.h
#pragma once
#include "EventMeta.h"
#include <nlohmann/json.hpp>

class Kline : public EventMeta {
public:
    // Flattened data fields mapped right from the JSON payload
    uint64_t startTime;
    uint64_t closeTime;
    std::string interval;
    double openPrice;
    double closePrice;
    double highPrice;
    double lowPrice;
    double volume;
    bool isClosed;

    // Takes the whole chunk of data at birth
    Kline(const nlohmann::json& payload);

    void print() const override;
};