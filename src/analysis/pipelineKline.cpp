#include "analysis/pipelineKline.h"
#include "analysis/MeanFactoryKline.h"
#include "analysis/ModeFactoryKline.h"
#include <iostream>
#include <thread>

ModeResult PipelineKline::ModeFactory(const Window<Kline>& window) {
    return ModeFactoryKline::compute(window);
}

MeanResult PipelineKline::MeanFactory(const Window<Kline>& window) {
    return MeanFactoryKline::compute(window);
}

void PipelineKline::runPipeline(const Window<Kline>& window) {
    MeanResult result;
    ModeResult mode_result;

    // Launch MeanFactory execution in a dedicated thread
    std::thread factoryThread([&window, &result]() {
        result = PipelineKline::MeanFactory(window);
    });

    // Launch ModeFactory execution in a dedicated thread
    std::thread modeFactoryThread([&window, &mode_result]() {
        mode_result = PipelineKline::ModeFactory(window);
    });

    // Wait for the factory thread to complete
    if (factoryThread.joinable()) {
        factoryThread.join();
    }

    if (modeFactoryThread.joinable()) {
        modeFactoryThread.join();
    }

    // Print the result after synchronization
    result.print();
    mode_result.print();
}
