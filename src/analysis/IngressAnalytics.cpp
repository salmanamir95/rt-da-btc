#include "IngressAnalytics.h"
#include <iostream>

void IngressAnalytics::run_pipeline_kline(){
    std::cout << "[+] Ingress Analytics Pipeline Started" << std::endl;
    while (true)
    {

        manager.is_allowed_analytics(EventID::KLINE);

        // 2. Trigger your multidimensional math thread joins
        //PipelineKline::runPipeline(manager.get_kline_window());

        // 3. Calculations done, release the lock on the KLINE lane
        manager.give_permission_to_proceed(EventID::KLINE);
        
    }
}

IngressAnalytics::IngressAnalytics(MemoryManager& memory) : memory(memory) {
    std::thread pipeKline(run_pipeline_kline());
    if (pipeKline.joinable()) {
        pipeKline.join();
    }

}