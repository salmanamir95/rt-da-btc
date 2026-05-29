#include "analysis/StdDevResult.h"
#include <iostream>

void StdDevResult::print() const {
    std::cout << "[Analytics] StdDevResult -> Time: " << time
              << " | StdDevOP: " << StdDevOP
              << " | StdDevHP: " << StdDevHP
              << " | StdDevLP: " << StdDevLP
              << " | StdDevCP: " << StdDevCP
              << " | StdDevVol: " << StdDevVol << std::endl;
}
