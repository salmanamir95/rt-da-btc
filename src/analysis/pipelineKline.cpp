#include "analysis/pipelineKline.h"
#include "analysis/MeanFactoryKline.h"
#include <iostream>

MeanResult PipelineKline::MeanFactory(const Window<Kline>& window) {
    return MeanFactoryKline::compute(window);
}

void PipelineKline::runPipeline(const Window<Kline>& window) {
    // Pipeline generates the mean result and prints it
    MeanResult result = PipelineKline::MeanFactory(window);
    result.print();
}
