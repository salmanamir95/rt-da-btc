#pragma once

#include "cache/window.h"
#include "ingestor/kline.h"
#include "analysis/MedianResult.h"
#include <thread>

class MedianFactoryKline {
public:
    static MedianResult compute(const Window<Kline>& window);

private:
    static void calcMedianOP(const Window<Kline>& window, double& result);
    static void calcMedianCP(const Window<Kline>& window, double& result);
    static void calcMedianHP(const Window<Kline>& window, double& result);
    static void calcMedianLP(const Window<Kline>& window, double& result);
    static void calcMedianVol(const Window<Kline>& window, double& result);
};
