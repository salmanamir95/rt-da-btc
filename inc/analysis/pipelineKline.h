#pragma once

#include "cache/window.h"
#include "ingestor/kline.h"
#include "analysis/MeanResult.h"
#include "analysis/ModeResult.h"
#include "analysis/MedianResult.h"

class PipelineKline {
public:
    static MedianResult MedianFactory(const Window<Kline>& window);
    static ModeResult ModeFactory(const Window<Kline>& window);

    // Easily callable static factory wrapper
    static MeanResult MeanFactory(const Window<Kline>& window);

    // The main execution trigger called by the analytics thread
    static void runPipeline(const Window<Kline>& window);
};