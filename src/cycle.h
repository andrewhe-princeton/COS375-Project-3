#pragma once
#include <string>

#include "cache.h"
#include "Utilities.h"
#include "emulator.h"

// init the emulator and all info
Status initSimulator(CacheConfig& icConfig, CacheConfig& dcConfig, MemoryStore* memory,
                     const std::string& output_name);

// run the emulator for a certain number of cycles
Status runCycles(uint32_t cycles);

// run till halt (call runCycles() with cycles == 1 each time) until
// status tells you to HALT or ERROR out
Status runTillHalt();

// dump the state of the emulator
Status finalizeSimulator();