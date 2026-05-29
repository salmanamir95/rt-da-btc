#pragma once
#include "ingestor/EventQueue.h"
#include <iostream>
#include "cache/memoryManager.h"

void run_cache(EventQueue& queue, MemoryManager& manager);
