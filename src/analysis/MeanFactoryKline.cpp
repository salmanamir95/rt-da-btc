#include "analysis/MeanFactoryKline.h"
#include <iostream>

void MeanFactoryKline::calcMeanOP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    if (count == 0) {
        result = 0.0;
        return;
    }

    // --- OPTIMIZATION: Zero-Branch Unrolled Speed Lane ---
    // If the matrix window is completely full (8/8 elements), compute 
    // the sum instantly without loop counters or branch mispredictions.
    if (count == 8) {
        result = (window.c0()->openPrice +
                  window.c1()->openPrice +
                  window.c2()->openPrice +
                  window.c3()->openPrice +
                  window.c4()->openPrice +
                  window.c5()->openPrice +
                  window.c6()->openPrice +
                  window.c7()->openPrice) / 8.0;
        return;
    }

    // --- WARMUP FALLBACK LANE ---
    // Handles the initial buffering phases (Elements 1 to 7) safely
    double sum = 0.0;
    if (window.c0()) sum += window.c0()->openPrice;
    if (window.c1()) sum += window.c1()->openPrice;
    if (window.c2()) sum += window.c2()->openPrice;
    if (window.c3()) sum += window.c3()->openPrice;
    if (window.c4()) sum += window.c4()->openPrice;
    if (window.c5()) sum += window.c5()->openPrice;
    if (window.c6()) sum += window.c6()->openPrice;
    if (window.c7()) sum += window.c7()->openPrice;

    result = sum / static_cast<double>(count);  
}

void MeanFactoryKline::calcMeanCP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    if (count == 0) {
        result = 0.0;
        return;
    }

    // --- OPTIMIZATION: Zero-Branch Unrolled Speed Lane ---
    // If the matrix window is completely full (8/8 elements), compute 
    // the sum instantly without loop counters or branch mispredictions.
    if (count == 8) {
        result = (window.c0()->closePrice +
                  window.c1()->closePrice +
                  window.c2()->closePrice +
                  window.c3()->closePrice +
                  window.c4()->closePrice +
                  window.c5()->closePrice +
                  window.c6()->closePrice +
                  window.c7()->closePrice) / 8.0;
        return;
    }

    // --- WARMUP FALLBACK LANE ---
    // Handles the initial buffering phases (Elements 1 to 7) safely
    double sum = 0.0;
    if (window.c0()) sum += window.c0()->closePrice;
    if (window.c1()) sum += window.c1()->closePrice;
    if (window.c2()) sum += window.c2()->closePrice;
    if (window.c3()) sum += window.c3()->closePrice;
    if (window.c4()) sum += window.c4()->closePrice;
    if (window.c5()) sum += window.c5()->closePrice;
    if (window.c6()) sum += window.c6()->closePrice;
    if (window.c7()) sum += window.c7()->closePrice;

    result = sum / static_cast<double>(count);
}

void MeanFactoryKline::calcMeanHP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    if (count == 0) {
        result = 0.0;
        return;
    }

    // --- OPTIMIZATION: Zero-Branch Unrolled Speed Lane ---
    // If the matrix window is completely full (8/8 elements), compute 
    // the sum instantly without loop counters or branch mispredictions.
    if (count == 8) {
        result = (window.c0()->highPrice +
                  window.c1()->highPrice +
                  window.c2()->highPrice +
                  window.c3()->highPrice +
                  window.c4()->highPrice +
                  window.c5()->highPrice +
                  window.c6()->highPrice +
                  window.c7()->highPrice) / 8.0;
        return;
    }

    // --- WARMUP FALLBACK LANE ---
    // Handles the initial buffering phases (Elements 1 to 7) safely
    double sum = 0.0;
    if (window.c0()) sum += window.c0()->highPrice;
    if (window.c1()) sum += window.c1()->highPrice;
    if (window.c2()) sum += window.c2()->highPrice;
    if (window.c3()) sum += window.c3()->highPrice;
    if (window.c4()) sum += window.c4()->highPrice;
    if (window.c5()) sum += window.c5()->highPrice;
    if (window.c6()) sum += window.c6()->highPrice;
    if (window.c7()) sum += window.c7()->highPrice;

    result = sum / static_cast<double>(count);    
}

void MeanFactoryKline::calcMeanLP(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    if (count == 0) {
        result = 0.0;
        return;
    }

    // --- OPTIMIZATION: Zero-Branch Unrolled Speed Lane ---
    // If the matrix window is completely full (8/8 elements), compute 
    // the sum instantly without loop counters or branch mispredictions.
    if (count == 8) {
        result = (window.c0()->lowPrice +
                  window.c1()->lowPrice +
                  window.c2()->lowPrice +
                  window.c3()->lowPrice +
                  window.c4()->lowPrice +
                  window.c5()->lowPrice +
                  window.c6()->lowPrice +
                  window.c7()->lowPrice) / 8.0;
        return;
    }

    // --- WARMUP FALLBACK LANE ---
    // Handles the initial buffering phases (Elements 1 to 7) safely
    double sum = 0.0;
    if (window.c0()) sum += window.c0()->lowPrice;
    if (window.c1()) sum += window.c1()->lowPrice;
    if (window.c2()) sum += window.c2()->lowPrice;
    if (window.c3()) sum += window.c3()->lowPrice;
    if (window.c4()) sum += window.c4()->lowPrice;
    if (window.c5()) sum += window.c5()->lowPrice;
    if (window.c6()) sum += window.c6()->lowPrice;
    if (window.c7()) sum += window.c7()->lowPrice;

    result = sum / static_cast<double>(count);  
}

void MeanFactoryKline::calcMeanVol(const Window<Kline>& window, double& result) {
    const size_t count = window.size();
    if (count == 0) {
        result = 0.0;
        return;
    }

    // --- OPTIMIZATION: Zero-Branch Unrolled Speed Lane ---
    // If the matrix window is completely full (8/8 elements), compute 
    // the sum instantly without loop counters or branch mispredictions.
    if (count == 8) {
        result = (window.c0()->volume +
                  window.c1()->volume +
                  window.c2()->volume +
                  window.c3()->volume +
                  window.c4()->volume +
                  window.c5()->volume +
                  window.c6()->volume +
                  window.c7()->volume) / 8.0;
        return;
    }

    // --- WARMUP FALLBACK LANE ---
    // Handles the initial buffering phases (Elements 1 to 7) safely
    double sum = 0.0;
    if (window.c0()) sum += window.c0()->volume;
    if (window.c1()) sum += window.c1()->volume;
    if (window.c2()) sum += window.c2()->volume;
    if (window.c3()) sum += window.c3()->volume;
    if (window.c4()) sum += window.c4()->volume;
    if (window.c5()) sum += window.c5()->volume;
    if (window.c6()) sum += window.c6()->volume;
    if (window.c7()) sum += window.c7()->volume;

    result = sum / static_cast<double>(count);  
}

MeanResult MeanFactoryKline::compute(const Window<Kline>& window) {
    MeanResult res;

    // Spawn independent threads for each dimension
    std::thread tOP(calcMeanOP, std::cref(window), std::ref(res.MeanOP));
    std::thread tCP(calcMeanCP, std::cref(window), std::ref(res.MeanCP));
    std::thread tHP(calcMeanHP, std::cref(window), std::ref(res.MeanHP));
    std::thread tLP(calcMeanLP, std::cref(window), std::ref(res.MeanLP));
    std::thread tVol(calcMeanVol, std::cref(window), std::ref(res.MeanVol));

    // Join threads back to synchronize the final result
    if (tOP.joinable()) tOP.join();
    if (tCP.joinable()) tCP.join();
    if (tHP.joinable()) tHP.join();
    if (tLP.joinable()) tLP.join();
    if (tVol.joinable()) tVol.join();


    if (window.c7()) res.time = window.c7()->closeTime;
    else if (window.c6()) res.time = window.c6()->closeTime;
    else if (window.c5()) res.time = window.c5()->closeTime;
    else if (window.c4()) res.time = window.c4()->closeTime;
    else if (window.c3()) res.time = window.c3()->closeTime;
    else if (window.c2()) res.time = window.c2()->closeTime;
    else if (window.c1()) res.time = window.c1()->closeTime;
    else if (window.c0()) res.time = window.c0()->closeTime;
    else res.time = 0; // Dummy time for now
    
    return res;
}
