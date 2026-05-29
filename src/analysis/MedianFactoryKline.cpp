#include "analysis/MedianFactoryKline.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>

static double calculateMedian(std::array<double, 8>& data, size_t count) {
    if (count == 0) return 0.0;
    if (count == 1) return data[0];

    std::sort(data.begin(), data.begin() + count);

    if (count % 2 == 0) {
        return (data[count / 2 - 1] + data[count / 2]) / 2.0;
    } else {
        return data[count / 2];
    }
}

void MedianFactoryKline::calcMedianOP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    std::array<double, 8> prices{};
    
    if (window.c0()) prices[0] = window.c0()->openPrice;
    if (window.c1()) prices[1] = window.c1()->openPrice;
    if (window.c2()) prices[2] = window.c2()->openPrice;
    if (window.c3()) prices[3] = window.c3()->openPrice;
    if (window.c4()) prices[4] = window.c4()->openPrice;
    if (window.c5()) prices[5] = window.c5()->openPrice;
    if (window.c6()) prices[6] = window.c6()->openPrice;
    if (window.c7()) prices[7] = window.c7()->openPrice;

    result = calculateMedian(prices, count);
}

void MedianFactoryKline::calcMedianCP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    std::array<double, 8> prices{};
    
    if (window.c0()) prices[0] = window.c0()->closePrice;
    if (window.c1()) prices[1] = window.c1()->closePrice;
    if (window.c2()) prices[2] = window.c2()->closePrice;
    if (window.c3()) prices[3] = window.c3()->closePrice;
    if (window.c4()) prices[4] = window.c4()->closePrice;
    if (window.c5()) prices[5] = window.c5()->closePrice;
    if (window.c6()) prices[6] = window.c6()->closePrice;
    if (window.c7()) prices[7] = window.c7()->closePrice;

    result = calculateMedian(prices, count);
}

void MedianFactoryKline::calcMedianHP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    std::array<double, 8> prices{};
    
    if (window.c0()) prices[0] = window.c0()->highPrice;
    if (window.c1()) prices[1] = window.c1()->highPrice;
    if (window.c2()) prices[2] = window.c2()->highPrice;
    if (window.c3()) prices[3] = window.c3()->highPrice;
    if (window.c4()) prices[4] = window.c4()->highPrice;
    if (window.c5()) prices[5] = window.c5()->highPrice;
    if (window.c6()) prices[6] = window.c6()->highPrice;
    if (window.c7()) prices[7] = window.c7()->highPrice;

    result = calculateMedian(prices, count);
}

void MedianFactoryKline::calcMedianLP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    std::array<double, 8> prices{};
    
    if (window.c0()) prices[0] = window.c0()->lowPrice;
    if (window.c1()) prices[1] = window.c1()->lowPrice;
    if (window.c2()) prices[2] = window.c2()->lowPrice;
    if (window.c3()) prices[3] = window.c3()->lowPrice;
    if (window.c4()) prices[4] = window.c4()->lowPrice;
    if (window.c5()) prices[5] = window.c5()->lowPrice;
    if (window.c6()) prices[6] = window.c6()->lowPrice;
    if (window.c7()) prices[7] = window.c7()->lowPrice;

    result = calculateMedian(prices, count);
}

void MedianFactoryKline::calcMedianVol(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    std::array<double, 8> volumes{};
    
    if (window.c0()) volumes[0] = window.c0()->volume;
    if (window.c1()) volumes[1] = window.c1()->volume;
    if (window.c2()) volumes[2] = window.c2()->volume;
    if (window.c3()) volumes[3] = window.c3()->volume;
    if (window.c4()) volumes[4] = window.c4()->volume;
    if (window.c5()) volumes[5] = window.c5()->volume;
    if (window.c6()) volumes[6] = window.c6()->volume;
    if (window.c7()) volumes[7] = window.c7()->volume;

    result = calculateMedian(volumes, count);
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

    if (window.c7()) result.time = window.c7()->closeTime;
    else if (window.c6()) result.time = window.c6()->closeTime;
    else if (window.c5()) result.time = window.c5()->closeTime;
    else if (window.c4()) result.time = window.c4()->closeTime;
    else if (window.c3()) result.time = window.c3()->closeTime;
    else if (window.c2()) result.time = window.c2()->closeTime;
    else if (window.c1()) result.time = window.c1()->closeTime;
    else if (window.c0()) result.time = window.c0()->closeTime;
    else result.time = 0; // Dummy time for now

    return result;
}
