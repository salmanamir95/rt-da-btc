#include "analysis/MedianResult.h"
#include <iostream>

void MedianResult::print() const {
    std::cout << "[Median Analysis | Outlier-Resistant Midpoint] Window Time: " << time
              << " | Open: " << MedianOP
              << " | High: " << MedianHP
              << " | Low: " << MedianLP
              << " | Close: " << MedianCP
              << " | Vol: " << MedianVol << std::endl;
}
