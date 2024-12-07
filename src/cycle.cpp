#include "cycle.h"

#include <iostream>
#include <memory>
#include <random>
#include <string>

#include "Utilities.h"
#include "cache.h"
#include "emulator.h"

static Emulator* emulator = nullptr;
static Cache* iCache = nullptr;
static Cache* dCache = nullptr;
static std::string output;
static uint32_t cycleCount = 0;
static uint32_t loadStalls = 0;
static PipeState pipeState = {0, 0, 0, 0, 0};


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
// NOTE Fix the bugs in this file first Ed#416
// 1. iCache access with info.pc instead of info.address ✔️
// 2. use dCache->config.missLatency instead of iCache's one when accessing dCache ✔️
// 3. maintaining pipe state across different calls to runCycles
// 4. look into the count and cycleCount variables.
// 5. correct time to set status to HALT
// 6. Add exception handling (ie overflow -> timing)
// 7. Add timing for different stalls (both load-use stalls and load-branch stalls)
Status runCycles(uint32_t cycles) {
    uint32_t count = 0;
    auto status = SUCCESS;    

    while (cycles == 0 || count < cycles) {
        Emulator::InstructionInfo info = emulator->executeInstruction();
        pipeState.cycle = cycleCount;  // get the execution cycle count
        
        // Fix later for hazards
        // Propagate instructions through the pipeline
        pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
        pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
        pipeState.exInstr = pipeState.idInstr;   // ID -> EX
        pipeState.idInstr = pipeState.ifInstr;   // IF -> ID
        pipeState.ifInstr = info.instruction;

        // uint32_t cacheDelay = 0;  // initially no delay for cache read/write

        // handle iCache delay
        cout << "infoPC: " << info.pc << endl;
        uint32_t iCacheDelay  = iCache->access(info.pc, CACHE_READ) ? 0 : iCache->config.missLatency;
        uint32_t dCacheDelay = 0;

        // handle dCache delays (in a multicycle style)
        if (info.isValid && (info.opcode == OP_LBU || info.opcode == OP_LHU || info.opcode == OP_LW)){
            dCacheDelay = dCache->access(info.address, CACHE_READ) ? 0 : dCache->config.missLatency;
        }

        if (info.isValid && (info.opcode == OP_SB || info.opcode == OP_SH || info.opcode == OP_SW)){
            dCacheDelay = dCache->access(info.address, CACHE_WRITE) ? 0 : dCache->config.missLatency;
        }
        
        while (iCacheDelay + dCacheDelay > 0) {
            cout << "iCache: " << iCacheDelay << endl;
            cout << "dCache: " << dCacheDelay << endl;

            dumpPipeState(pipeState, output);
            cycleCount++;
            pipeState.cycle = cycleCount;

            
            count++;
            if (iCacheDelay > 0 && dCacheDelay > 0){
                iCacheDelay--;
                dCacheDelay--;
                pipeState.wbInstr = 0;
            }
            else if (dCacheDelay > 0){
                pipeState.wbInstr = 0;
                dCacheDelay--;
            }
            else if (iCacheDelay){
                pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                pipeState.exInstr = pipeState.idInstr;   // ID -> EX
                pipeState.idInstr = 0;
                iCacheDelay--;
            }
        }
        //iCacheDelay = 0;
        //dCacheDelay = 0;
        //pipeState.cycle = cycleCount;
        count ++;
        cycleCount++;

        // halting on first entry in IF -> should finish WB and then dumpPipeState
        if (info.isHalt) {
            status = HALT;
            break;
        }

        dumpPipeState(pipeState, output);

    }

    // HALT handling here?

    // Not exactly the right way, just a demonstration here
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
    SimulationStats stats{ emulator->getDin(), cycleCount, iCache->getHits(), iCache->getMisses(),
                                                           dCache->getHits(), dCache->getMisses(), loadStalls};  // TODO: Incomplete Implementation
    dumpSimStats(stats, output);
    return SUCCESS;
}
