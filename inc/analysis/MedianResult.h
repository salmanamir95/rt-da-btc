#pragma once
#include <cstdint>

struct MedianResult {
    double MedianOP;
    double MedianCP;
    double MedianHP;
    double MedianLP;
    double MedianVol;
    uint64_t time;

    void print() const;
};
