#include "analysis/MeanResult.h"
#include <iostream>

void MeanResult::print() const {
    std::cout << "[Mean Analysis | Moving Average Trend] Window Time: " << time
              << " | Open: " << MeanOP
              << " | High: " << MeanHP
              << " | Low: " << MeanLP
              << " | Close: " << MeanCP
              << " | Vol: " << MeanVol << std::endl;
}
