#include "analysis/MedianResult.h"
#include <iostream>

void MedianResult::print() const {
    std::cout << "[Analytics] MedianResult -> Time: " << time
              << " | MedianOP: " << MedianOP
              << " | MedianHP: " << MedianHP
              << " | MedianLP: " << MedianLP
              << " | MedianCP: " << MedianCP
              << " | MedianVol: " << MedianVol << std::endl;
}
