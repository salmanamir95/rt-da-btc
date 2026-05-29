#include "analysis/pipelineKline.h"
#include "analysis/MeanFactoryKline.h"
#include <iostream>
#include <thread>

MeanResult PipelineKline::MeanFactory(const Window<Kline>& window) {
    return MeanFactoryKline::compute(window);
}

void PipelineKline::runPipeline(const Window<Kline>& window) {
    MeanResult result;

    // Launch MeanFactory execution in a dedicated thread
    std::thread factoryThread([&window, &result]() {
        result = PipelineKline::MeanFactory(window);
    });

    // Wait for the factory thread to complete
    if (factoryThread.joinable()) {
        factoryThread.join();
    }

    // Print the result after synchronization
    result.print();
}
