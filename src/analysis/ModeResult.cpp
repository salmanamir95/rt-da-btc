#include "analysis/ModeResult.h"
#include <iostream>


void ModeResult::print() const {
    std::cout << "[Mode Analysis | Support/Resistance Clusters] Window Time: " << time
              << " | Open: " << ModeOP
              << " | High: " << ModeHP
              << " | Low: " << ModeLP
              << " | Close: " << ModeCP
              << " | Vol: " << ModeVol << std::endl;
}