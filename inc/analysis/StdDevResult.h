#pragma once
#include <cstdint>

struct StdDevResult {
    double StdDevOP;
    double StdDevCP;
    double StdDevHP;
    double StdDevLP;
    double StdDevVol;
    uint64_t time;

    void print() const;
};
