#pragma once
#include <string>

#include "Utilities.h"
#include "emulator.h"

// init the emulator and all info
Status initEmulator(MemoryStore* memory, const std::string& output_name);

// run the emulator for a certain number of instructions
Status runInstructions(uint32_t instructions);

// run till halt (call runInstructions() with instructions == 1 each time) until
// status tells you to HALT or ERROR out
Status runTillHalt();

// dump the state of the emulator
Status finalizeEmulator();
