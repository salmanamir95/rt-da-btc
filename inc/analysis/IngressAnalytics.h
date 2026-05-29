#pragma once

#include "cache/memory/memoryManager.h"
#include <thread>

class IngressAnalytics{
    MemoryManager& memory;
    public:
        IngressAnalytics(MemoryManager& memory);

        void run_pipeline_kline();
        void run_pipeline_bo();

};