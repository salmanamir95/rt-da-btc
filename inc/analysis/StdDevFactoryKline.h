#pragma once

#include "cache/window.h"
#include "ingestor/kline.h"
#include "analysis/StdDevResult.h"
#include <thread>

class StdDevFactoryKline {
public:
    static StdDevResult compute(const Window<Kline>& window);

private:
    static void calcStdDevOP(const Window<Kline>& window, double& result);
    static void calcStdDevCP(const Window<Kline>& window, double& result);
    static void calcStdDevHP(const Window<Kline>& window, double& result);
    static void calcStdDevLP(const Window<Kline>& window, double& result);
    static void calcStdDevVol(const Window<Kline>& window, double& result);
};
