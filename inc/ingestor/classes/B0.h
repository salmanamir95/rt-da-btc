#pragma once
#include "EventMeta.h"
#include <nlohmann/json.hpp>

class B0 : public EventMeta {
public:
    B0(const nlohmann::json& payload);
    void process() override;
    void print() const override;
};
