#include "funct.h"

#include <iostream>

#include "cache.h"
#include "Utilities.h"
#include "emulator.h"

static Emulator* emulator = nullptr;
static std::string output;

// initialize the emulator
Status initEmulator(MemoryStore* mem, const std::string& output_name) {
    output = output_name;
    emulator = new Emulator();
    emulator->setMemory(mem);
    return SUCCESS;
}

// run the emulator for a certain number of intructions
// return SUCCESS if count of executed instructions == desired intructions.
// return HALT if the simulator halts on 0xfeedfeed
Status runInstructions(uint32_t instructions) {
    uint32_t numInstructions = 0;
    auto status = SUCCESS;

    while (instructions == 0 || numInstructions < instructions) {
        Emulator::InstructionInfo info = emulator->executeInstruction();

        numInstructions += 1;

        if (info.isHalt) {
            status = HALT;
            break;
        }
    }
    return status;
}

// run till halt (call runInstructions() with instructions == 1 each time) until
// status tells you to HALT or ERROR out
Status runTillHalt() {
    Status status;
    while (true) {
        status = static_cast<Status>(runInstructions(1));
        if (status == HALT) break;
    }
    return status;
}

// dump the stats of the emulator
Status finalizeEmulator() {
    emulator->dumpRegMem(output);
    SimulationStats stats{emulator->getDin(), 0,};
    dumpSimStats(stats, output);
    return SUCCESS;
}
