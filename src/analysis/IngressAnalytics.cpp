#include "analysis/IngressAnalytics.h"
#include "analysis/pipelineKline.h"
#include <iostream>

void IngressAnalytics::run_pipeline_kline(){
    std::cout << "[+] Ingress Analytics Pipeline Started" << std::endl;
    while (true)
    {
        std::cout << "[Analytics] Ask permission to compute" << std::endl;

        memory.is_allowed_analytics(EventID::KLINE);

        std::cout << "[Analytics] Computing Kline Pipelines" << std::endl;

        // 2. Trigger your multidimensional math thread joins
        PipelineKline::runPipeline(memory.get_kline_window());

        std::cout << "[Analytics] Done with KLINE Pipelines, Releasing Lock" << std::endl;

        // 3. Calculations done, release the lock on the KLINE lane
        memory.give_permission_to_proceed(EventID::KLINE);
        
    }
}

IngressAnalytics::IngressAnalytics(MemoryManager& memory) : memory(memory) {
    std::thread pipeKline(&IngressAnalytics::run_pipeline_kline, this);
    if (pipeKline.joinable()) {
        pipeKline.join();
    }

}