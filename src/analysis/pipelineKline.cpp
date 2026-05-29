#include "analysis/pipelineKline.h"
#include "analysis/MeanFactoryKline.h"
#include "analysis/ModeFactoryKline.h"
#include "analysis/MedianFactoryKline.h"
#include <iostream>
#include <thread>

MedianResult PipelineKline::MedianFactory(const Window<Kline>& window) {
    return MedianFactoryKline::compute(window);
}

ModeResult PipelineKline::ModeFactory(const Window<Kline>& window) {
    return ModeFactoryKline::compute(window);
}

MeanResult PipelineKline::MeanFactory(const Window<Kline>& window) {
    return MeanFactoryKline::compute(window);
}

void PipelineKline::runPipeline(const Window<Kline>& window) {
    MeanResult result;
    ModeResult mode_result;
    MedianResult median_result;

    // Launch MeanFactory execution in a dedicated thread
    std::thread factoryThread([&window, &result]() {
        result = PipelineKline::MeanFactory(window);
    });

    // Launch ModeFactory execution in a dedicated thread
    std::thread modeFactoryThread([&window, &mode_result]() {
        mode_result = PipelineKline::ModeFactory(window);
    });

    // Launch MedianFactory execution in a dedicated thread
    std::thread medianFactoryThread([&window, &median_result]() {
        median_result = PipelineKline::MedianFactory(window);
    });

    // Wait for the factory threads to complete
    if (factoryThread.joinable()) {
        factoryThread.join();
    }

    if (modeFactoryThread.joinable()) {
        modeFactoryThread.join();
    }

    if (medianFactoryThread.joinable()) {
        medianFactoryThread.join();
    }

    // Print the result after synchronization
    result.print();
    mode_result.print();
    median_result.print();
}
