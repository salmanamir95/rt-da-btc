#include "analysis/ModeFactoryKline.h"
#include <iostream>
#include <array>
#include <cmath>
#include <vector>

// Helper to calculate the mode of a hardcoded 8-element floating point stack array
// Bins values to eliminate tick-level floating noise.
static double calculateStackMode(const std::array<double, 8>& data, size_t count) {
    if (count == 0) return 0.0;

    struct BinFrequency {
        int binned_value = 0;
        size_t count = 0;
        double raw_representative = 0.0;
    };

    // Explicitly stack-allocated tracker array (64 bytes - fits a single cache line)
    std::array<BinFrequency, 8> freq_map{};
    size_t unique_bins = 0;
    size_t max_freq = 0;
    double modal_candidate = data[0];

    for (size_t i = 0; i < count; ++i) {
        // Binning to nearest integer unit (e.g. nearest $1 for BTC) to find price cluster walls
        int binned = static_cast<int>(std::round(data[i]));
        bool matched = false;

        for (size_t j = 0; j < unique_bins; ++j) {
            if (freq_map[j].binned_value == binned) {
                freq_map[j].count++;
                matched = true;
                if (freq_map[j].count > max_freq) {
                    max_freq = freq_map[j].count;
                    modal_candidate = freq_map[j].raw_representative;
                }
                break;
            }
        }

        if (!matched) {
            freq_map[unique_bins] = {binned, 1, data[i]};
            if (max_freq == 0) {
                max_freq = 1;
                modal_candidate = data[i];
            }
            unique_bins++;
        }
    }

    // EDGE CASE: If max_freq == 1, every price point is completely unique in the window.
    // To prevent data skewing on your dashboard, we fallback to the mid-window arithmetic average.
    if (max_freq == 1) {
        double sum = 0.0;
        for (size_t i = 0; i < count; ++i) sum += data[i];
        return sum / static_cast<double>(count);
    }

    return modal_candidate;
}

void ModeFactoryKline::calcModeOP(const Window<Kline>& window, double& result) {
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

    result = calculateStackMode(prices, count);
}

void ModeFactoryKline::calcModeCP(const Window<Kline>& window, double& result) {
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

    result = calculateStackMode(prices, count);
}

void ModeFactoryKline::calcModeHP(const Window<Kline>& window, double& result) {
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

    result = calculateStackMode(prices, count);
}

void ModeFactoryKline::calcModeLP(const Window<Kline>& window, double& result) {
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

    result = calculateStackMode(prices, count);
}

void ModeFactoryKline::calcModeVol(const Window<Kline>& window, double& result) {
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

    result = calculateStackMode(volumes, count);
}

ModeResult ModeFactoryKline::compute(const Window<Kline>& window) {
    ModeResult result;
    std::vector<std::thread> threads;
    threads.reserve(5);

    // Dynamic extraction time logic mapping from window head 
    if (window.c0()) {
        result.time = static_cast<double>(window.c0()->startTime); 
    } else {
        result.time = 0.0;
    }

    // Fixed internal reference destinations to properly match header metrics fields
    threads.emplace_back(&ModeFactoryKline::calcModeOP, std::cref(window), std::ref(result.ModeOP));
    threads.emplace_back(&ModeFactoryKline::calcModeCP, std::cref(window), std::ref(result.ModeCP)); 
    threads.emplace_back(&ModeFactoryKline::calcModeHP, std::cref(window), std::ref(result.ModeHP));
    threads.emplace_back(&ModeFactoryKline::calcModeLP, std::cref(window), std::ref(result.ModeLP));
    threads.emplace_back(&ModeFactoryKline::calcModeVol, std::cref(window), std::ref(result.ModeVol));

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return result;
}