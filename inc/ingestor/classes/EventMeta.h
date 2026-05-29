#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "EventTypes.h"

class EventMeta {
public:
    uint64_t timestamp; 
    std::string symbol;
    std::string eventType;
    // --- The New Core Type ID Data Member ---
    const uint8_t type_id;  

    // Constructor that enforces type_id assignment at birth
    explicit EventMeta(const uint8_t id) : type_id(id) {}

    // Virtual destructor
    virtual ~EventMeta() = default;
    
    virtual void process() = 0; 
    virtual void print() const = 0;
    
};