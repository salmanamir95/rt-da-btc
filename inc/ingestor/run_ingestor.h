#pragma once

#include <string>
#include "ingestor/EventQueue.h"

void run_ingestor(std::string url, EventQueue& queue);
