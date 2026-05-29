#pragma once

#include "cache/memory/window.h"
#include "kline.h"
#include "analysis/MeanResult.h"
#include <thread>

class MeanFactoryKline {
public:
    // Computes all statistics by delegating to multiple threads
    static MeanResult compute(const Window<Kline>& window);

private:
    // Sub-tasks for multi-threading calculations
    static void calcMeanOP(const Window<Kline>& window, double& result);
    static void calcMeanCP(const Window<Kline>& window, double& result);
    static void calcMeanHP(const Window<Kline>& window, double& result);
    static void calcMeanLP(const Window<Kline>& window, double& result);
    static void calcMeanVol(const Window<Kline>& window, double& result);
};
