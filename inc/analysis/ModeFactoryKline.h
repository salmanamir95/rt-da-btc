#pragma once

#include "cache/window.h"
#include "ingestor/kline.h"
#include "analysis/ModeResult.h"
#include <thread>

class ModeFactoryKline {
public:
    // Computes all statistics by delegating to multiple threads
    static ModeResult compute(const Window<Kline>& window);

private:
    // Sub-tasks for multi-threading calculations
    static void calcModeOP(const Window<Kline>& window, double& result);
    static void calcModeCP(const Window<Kline>& window, double& result);
    static void calcModeHP(const Window<Kline>& window, double& result);
    static void calcModeLP(const Window<Kline>& window, double& result);
    static void calcModeVol(const Window<Kline>& window, double& result);
};
