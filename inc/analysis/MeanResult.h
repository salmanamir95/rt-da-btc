#pragma once
#include <cstdint>

struct MeanResult {
    double MeanOP;
    double MeanCP;
    double MeanHP;
    double MeanLP;
    double MeanVol;
    uint64_t time;

    void print() const;
};
