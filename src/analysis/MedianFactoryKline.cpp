#include "analysis/MedianFactoryKline.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>

static double calculateMedian(std::array<double, 8>& data, size_t count) {
    if (count == 0) return 0.0;
    if (count == 1) return data[0];

    // Array is guaranteed contiguously packed from index 0 to count-1
    std::sort(data.begin(), data.begin() + count);

    if (count % 2 == 0) {
        return (data[count / 2 - 1] + data[count / 2]) / 2.0;
    } else {
        return data[count / 2];
    }
}

void MedianFactoryKline::calcMedianOP(const Window<Kline>& window, double& result) {
    std::array<double, 8> prices{};
    size_t active_count = 0;
    
    // Contiguously densify active data elements to avoid structural holes
    if (window.c0()) prices[active_count++] = window.c0()->openPrice;
    if (window.c1()) prices[active_count++] = window.c1()->openPrice;
    if (window.c2()) prices[active_count++] = window.c2()->openPrice;
    if (window.c3()) prices[active_count++] = window.c3()->openPrice;
    if (window.c4()) prices[active_count++] = window.c4()->openPrice;
    if (window.c5()) prices[active_count++] = window.c5()->openPrice;
    if (window.c6()) prices[active_count++] = window.c6()->openPrice;
    if (window.c7()) prices[active_count++] = window.c7()->openPrice;

    result = calculateMedian(prices, active_count);
}

void MedianFactoryKline::calcMedianCP(const Window<Kline>& window, double& result) {
    std::array<double, 8> prices{};
    size_t active_count = 0;
    
    if (window.c0()) prices[active_count++] = window.c0()->closePrice;
    if (window.c1()) prices[active_count++] = window.c1()->closePrice;
    if (window.c2()) prices[active_count++] = window.c2()->closePrice;
    if (window.c3()) prices[active_count++] = window.c3()->closePrice;
    if (window.c4()) prices[active_count++] = window.c4()->closePrice;
    if (window.c5()) prices[active_count++] = window.c5()->closePrice;
    if (window.c6()) prices[active_count++] = window.c6()->closePrice;
    if (window.c7()) prices[active_count++] = window.c7()->closePrice;

    result = calculateMedian(prices, active_count);
}

void MedianFactoryKline::calcMedianHP(const Window<Kline>& window, double& result) {
    std::array<double, 8> prices{};
    size_t active_count = 0;
    
    if (window.c0()) prices[active_count++] = window.c0()->highPrice;
    if (window.c1()) prices[active_count++] = window.c1()->highPrice;
    if (window.c2()) prices[active_count++] = window.c2()->highPrice;
    if (window.c3()) prices[active_count++] = window.c3()->highPrice;
    if (window.c4()) prices[active_count++] = window.c4()->highPrice;
    if (window.c5()) prices[active_count++] = window.c5()->highPrice;
    if (window.c6()) prices[active_count++] = window.c6()->highPrice;
    if (window.c7()) prices[active_count++] = window.c7()->highPrice;

    result = calculateMedian(prices, active_count);
}

void MedianFactoryKline::calcMedianLP(const Window<Kline>& window, double& result) {
    std::array<double, 8> prices{};
    size_t active_count = 0;
    
    if (window.c0()) prices[active_count++] = window.c0()->lowPrice;
    if (window.c1()) prices[active_count++] = window.c1()->lowPrice;
    if (window.c2()) prices[active_count++] = window.c2()->lowPrice;
    if (window.c3()) prices[active_count++] = window.c3()->lowPrice;
    if (window.c4()) prices[active_count++] = window.c4()->lowPrice;
    if (window.c5()) prices[active_count++] = window.c5()->lowPrice;
    if (window.c6()) prices[active_count++] = window.c6()->lowPrice;
    if (window.c7()) prices[active_count++] = window.c7()->lowPrice;

    result = calculateMedian(prices, active_count);
}

void MedianFactoryKline::calcMedianVol(const Window<Kline>& window, double& result) {
    std::array<double, 8> volumes{};
    size_t active_count = 0;
    
    if (window.c0()) volumes[active_count++] = window.c0()->volume;
    if (window.c1()) volumes[active_count++] = window.c1()->volume;
    if (window.c2()) volumes[active_count++] = window.c2()->volume;
    if (window.c3()) volumes[active_count++] = window.c3()->volume;
    if (window.c4()) volumes[active_count++] = window.c4()->volume;
    if (window.c5()) volumes[active_count++] = window.c5()->volume;
    if (window.c6()) volumes[active_count++] = window.c6()->volume;
    if (window.c7()) volumes[active_count++] = window.c7()->volume;

    result = calculateMedian(volumes, active_count);
}

MedianResult MedianFactoryKline::compute(const Window<Kline>& window) {
    MedianResult result;
    std::vector<std::thread> threads;
    threads.reserve(5);

    threads.emplace_back(&MedianFactoryKline::calcMedianOP, std::cref(window), std::ref(result.MedianOP));
    threads.emplace_back(&MedianFactoryKline::calcMedianCP, std::cref(window), std::ref(result.MedianCP)); 
    threads.emplace_back(&MedianFactoryKline::calcMedianHP, std::cref(window), std::ref(result.MedianHP));
    threads.emplace_back(&MedianFactoryKline::calcMedianLP, std::cref(window), std::ref(result.MedianLP));
    threads.emplace_back(&MedianFactoryKline::calcMedianVol, std::cref(window), std::ref(result.MedianVol));

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // High-performance time extraction fall-through block
    if (window.c7()) result.time = static_cast<double>(window.c7()->closeTime);
    else if (window.c6()) result.time = static_cast<double>(window.c6()->closeTime);
    else if (window.c5()) result.time = static_cast<double>(window.c5()->closeTime);
    else if (window.c4()) result.time = static_cast<double>(window.c4()->closeTime);
    else if (window.c3()) result.time = static_cast<double>(window.c3()->closeTime);
    else if (window.c2()) result.time = static_cast<double>(window.c2()->closeTime);
    else if (window.c1()) result.time = static_cast<double>(window.c1()->closeTime);
    else if (window.c0()) result.time = static_cast<double>(window.c0()->closeTime);
    else result.time = 0.0; 

    return result;
}