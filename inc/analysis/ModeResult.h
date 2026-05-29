#pragma once
#include <cstdint>

struct ModeResult {
    double ModeOP;
    double ModeCP;
    double ModeHP;
    double ModeLP;
    double ModeVol;
    uint64_t time;

    void print() const;
};
