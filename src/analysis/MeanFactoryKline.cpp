#include "analysis/MeanFactoryKline.h"
#include <iostream>

void MeanFactoryKline::calcMeanOP(const Window<Kline>& window, double& result) {
    // Dummy logic
    result = 42.1;
}

void MeanFactoryKline::calcMeanCP(const Window<Kline>& window, double& result) {
    // Dummy logic
    result = 42.2;
}

void MeanFactoryKline::calcMeanHP(const Window<Kline>& window, double& result) {
    // Dummy logic
    result = 42.3;
}

void MeanFactoryKline::calcMeanLP(const Window<Kline>& window, double& result) {
    // Dummy logic
    result = 42.4;
}

void MeanFactoryKline::calcMeanVol(const Window<Kline>& window, double& result) {
    // Dummy logic
    result = 42.5;
}

MeanResult MeanFactoryKline::compute(const Window<Kline>& window) {
    MeanResult res;
    res.time = 0; // Dummy time for now

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

    return res;
}
