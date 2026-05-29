#include "analysis/MeanResult.h"
#include <iostream>

void MeanResult::print() const {
    std::cout << "[Analytics] Dummy MeanResult -> Time: " << time
              << " | MeanOP: " << MeanOP
              << " | MeanHP: " << MeanHP
              << " | MeanLP: " << MeanLP
              << " | MeanCP: " << MeanCP
              << " | MeanVol: " << MeanVol << std::endl;
}
