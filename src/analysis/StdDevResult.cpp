#include "analysis/StdDevResult.h"
#include <iostream>

void StdDevResult::print() const {
    std::cout << "[StdDev Analysis | Market Volatility Index] Window Time: " << time
              << " | Open: " << StdDevOP
              << " | High: " << StdDevHP
              << " | Low: " << StdDevLP
              << " | Close: " << StdDevCP
              << " | Vol: " << StdDevVol << std::endl;
}
