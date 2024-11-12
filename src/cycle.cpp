#include "cycle.h"

#include <iostream>
#include <memory>
#include <string>

#include "Utilities.h"
#include "cache.h"
#include "emulator.h"

static Emulator* emulator = nullptr;
static Cache* iCache = nullptr;
static Cache* dCache = nullptr;
static std::string output;
static uint32_t cycleCount;


// NOTE: The list of places in the source code that are marked ToDo might not be comprehensive.
// Please keep this in mind as you work on the project.

/**TODO: 
 * Implement the pipeline emulation for the MIPS machine in this section.
 * A basic template is provided below but it doesn't account for all possible stalls and hazards as is
 */

// initialize the emulator
Status initSimulator(CacheConfig& iCacheConfig, CacheConfig& dCacheConfig, MemoryStore* mem,
                     const std::string& output_name) {
    output = output_name;
    emulator = new Emulator();
    emulator->setMemory(mem);
    iCache = new Cache(iCacheConfig, I_CACHE);
    dCache = new Cache(dCacheConfig, D_CACHE);
    return SUCCESS;
}

// Run the emulator for a certain number of cycles
// return HALT if the simulator halts on 0xfeedfeed
// return SUCCESS if we have executed the desired number of cycles
Status runCycles(uint32_t cycles) {
    uint32_t count = 0;
    auto status = SUCCESS;
    PipeState pipeState = {0,};

    while (cycles == 0 || count < cycles) {
        Emulator::InstructionInfo info = emulator->executeInstruction();
        pipeState.cycle = cycleCount;  // get the execution cycle count
        pipeState.ifInstr = info.instruction;

        uint32_t cacheDelay = 0;  // initially no delay for cache read/write

        // handle iCache delay
        cacheDelay += iCache->access(info.address, CACHE_READ) ? 0 : iCache->config.missLatency;

        // handle dCache delays (in a multicycle style)
        if (info.isValid && (info.opcode == OP_LBU || info.opcode == OP_LHU || info.opcode == OP_LW))
            cacheDelay += dCache->access(info.address, CACHE_READ) ? 0 : iCache->config.missLatency;

        if (info.isValid && (info.opcode == OP_SB || info.opcode == OP_SH || info.opcode == OP_SW))
            cacheDelay += dCache->access(info.address, CACHE_WRITE) ? 0 : iCache->config.missLatency;

        count += 1 + cacheDelay;
        cycleCount += 1 + cacheDelay;

        if (info.isHalt) {
            status = HALT;
            break;
        }
    }

    // Not exactly the right way, just a demonstration here
    dumpPipeState(pipeState, output);
    return status;
}

// run till halt (call runCycles() with cycles == 1 each time) until
// status tells you to HALT or ERROR out
Status runTillHalt() {
    Status status;
    while (true) {
        status = static_cast<Status>(runCycles(1));
        if (status == HALT) break;
    }
    return status;
}

// dump the state of the emulator
Status finalizeSimulator() {
    emulator->dumpRegMem(output);
    SimulationStats stats{ emulator->getDin(), cycleCount, };  // TODO: Incomplete Implementation
    dumpSimStats(stats, output);
    return SUCCESS;
}
