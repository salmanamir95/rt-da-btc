#include "analysis/ModeResult.h"
#include <iostream>


void ModeResult::print() const {
    std::cout << "[Analytics] ModeResult -> Time: " << time
              << " | ModeOP: " << ModeOP
              << " | ModeHP: " << ModeHP
              << " | ModeLP: " << ModeLP
              << " | ModeCP: " << ModeCP
              << " | ModeVol: " << ModeVol << std::endl;
}