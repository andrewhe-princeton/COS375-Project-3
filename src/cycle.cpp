#include "cycle.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include <string>

#include "Utilities.h"
#include "cache.h"
#include "emulator.h"

// Pipestate save
struct PipeInsInfo {
    Emulator::InstructionInfo ifInstr;
    Emulator::InstructionInfo idInstr;
    Emulator::InstructionInfo exInstr;
    Emulator::InstructionInfo memInstr;
    Emulator::InstructionInfo wbInstr;
};


static Emulator* emulator = nullptr;
static Cache* iCache = nullptr;
static Cache* dCache = nullptr;
static std::string output;
static uint32_t cycleCount = 0;
static uint32_t loadStalls = 0;
static PipeState pipeState = {0, 0, 0, 0, 0};
static PipeInsInfo pipeInsInfo;


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
// 3. maintaining pipe state across different calls to runCycles ✔️
// 4. look into the count and cycleCount variables. ✔️
// 5. correct time to set status to HALT ✔️
// 6. Add exception handling (ie overflow -> timing)
// 7. Add timing for different stalls (both load-use stalls and load-branch stalls)
// 8. Test count (count++ is correct?)
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

        // Pipestate with info
        pipeInsInfo.wbInstr = pipeInsInfo.memInstr;  // MEM -> WB
        pipeInsInfo.memInstr = pipeInsInfo.exInstr;  // EX -> MEM
        pipeInsInfo.exInstr = pipeInsInfo.idInstr;   // ID -> EX
        pipeInsInfo.idInstr = pipeInsInfo.ifInstr;   // IF -> ID
        pipeInsInfo.ifInstr = info;

        // Exception Handling
        if (!info.isValid || info.isOverflow) {
            // bug found in IF

            // push excepting instruction to ID
            dumpPipeState(pipeState, output);
            // push everything else
            pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
            pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
            pipeState.exInstr = pipeState.idInstr;   // ID -> EX
            pipeState.idInstr = pipeState.ifInstr;
            pipeState.ifInstr = 0;

            pipeInsInfo.wbInstr = pipeInsInfo.memInstr;  // MEM -> WB
            pipeInsInfo.memInstr = pipeInsInfo.exInstr;  // EX -> MEM
            pipeInsInfo.exInstr = pipeInsInfo.idInstr;   // ID -> EX
            pipeInsInfo.idInstr = pipeInsInfo.ifInstr;
            pipeInsInfo.ifInstr = Emulator::InstructionInfo();

            cycleCount++;
            pipeState.cycle = cycleCount;
            
            if (!info.isValid){
                // insert nops & propagate the exception from IF
                    
                // ------ squash the excepting instruction
                dumpPipeState(pipeState, output);
                // pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                // pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                // pipeState.exInstr = pipeState.idInstr;   // ID -> EX
                pipeState.idInstr = 0;

                // pipeInsInfo.wbInstr = pipeInsInfo.memInstr;  // MEM -> WB
                // pipeInsInfo.memInstr = pipeInsInfo.exInstr;  // EX -> MEM
                // pipeInsInfo.exInstr = pipeInsInfo.idInstr;   // ID -> EX
                pipeInsInfo.idInstr = Emulator::InstructionInfo();
                
                cycleCount++;
                pipeState.cycle = cycleCount;
                
                // exception officially found in ID  - continue as normal bc next fetch will be the exception address 0x8000
                continue;    
                
            }
            else if (info.isOverflow)  {
                
                // push excepting instruction till EX stage
                for (uint32_t i=0; i<1; ++i){
                    dumpPipeState(pipeState, output);
                    // push everything else
                    pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                    pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                    pipeState.exInstr = pipeState.idInstr;   // ID -> EX
                    pipeState.idInstr = pipeState.ifInstr;
                    pipeState.ifInstr = 0;

                    pipeInsInfo.wbInstr = pipeInsInfo.memInstr;  // MEM -> WB
                    pipeInsInfo.memInstr = pipeInsInfo.exInstr;  // EX -> MEM
                    pipeInsInfo.exInstr = pipeInsInfo.idInstr;   // ID -> EX
                    pipeInsInfo.idInstr = pipeInsInfo.ifInstr;
                    pipeInsInfo.ifInstr = Emulator::InstructionInfo();
                    cycleCount++;
                    pipeState.cycle = cycleCount;
                }

                // ------ squash the excepting instruction
                dumpPipeState(pipeState, output);
                // pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                pipeState.exInstr = 0; 
                
                // pipeInsInfo.wbInstr = pipeInsInfo.memInstr;  // MEM -> WB
                pipeInsInfo.exInstr = Emulator::InstructionInfo();  
                                
                cycleCount++;
                pipeState.cycle = cycleCount;

                // exception officially found in EX - continue as normal because next fetch will be the exception address 0x8000
                continue;
            
            }
        }
    
        
        // handle iCache delay
        uint32_t iCacheDelay = iCache->access(info.pc, CACHE_READ) ? 0 : iCache->config.missLatency;
        // handle dCache delay
        uint32_t dCacheDelay = 0;

        // handle dCache delays (in a multicycle style)
        if (pipeInsInfo.memInstr.isValid && (pipeInsInfo.memInstr.opcode == OP_LBU || pipeInsInfo.memInstr.opcode == OP_LHU || pipeInsInfo.memInstr.opcode == OP_LW)){
            dCacheDelay = dCache->access(pipeInsInfo.memInstr.loadAddress, CACHE_READ) ? 0 : dCache->config.missLatency;
        }

        if (pipeInsInfo.memInstr.isValid && (pipeInsInfo.memInstr.opcode == OP_SB || pipeInsInfo.memInstr.opcode == OP_SH || pipeInsInfo.memInstr.opcode == OP_SW)){
            dCacheDelay = dCache->access(pipeInsInfo.memInstr.storeAddress, CACHE_WRITE) ? 0 : dCache->config.missLatency;
        }

        // check for branch in ID
        // if branch in ID, check whether there is a branch-use reliance between branch in ID and instruction in EX.
        // stall one until instruction reaches MEM
        OP_IDS rt_CheckOps[] = {OP_ADDI, OP_ADDIU, OP_ANDI, OP_LBU, OP_LHU, OP_LUI, OP_LW, OP_ORI, OP_SLTI, OP_SLTIU};

        FUNCT_IDS rd_CheckOps[] = {FUN_ADD, FUN_ADDU, FUN_AND, FUN_NOR, FUN_OR, FUN_SLT, FUN_SLTU, FUN_SLL, FUN_SRL, FUN_SUB, FUN_SUBU};
        

        if (pipeInsInfo.idInstr.opcode == OP_BEQ || pipeInsInfo.idInstr.opcode == OP_BGTZ || pipeInsInfo.idInstr.opcode == OP_BLEZ || pipeInsInfo.idInstr.opcode == OP_BNE) {
            cout << "Branch foundaaa " << endl;
            cout << "RS: " << pipeInsInfo.idInstr.rs << endl;
            cout << "RD: " << pipeInsInfo.exInstr.rd << endl;
            cout << "RT: " << pipeInsInfo.exInstr.rt << endl;

            bool check_rt = false;
            bool check_rd = false;
            
            // checking RT - returns true if opcode is within our list above that modifies RT based on greensheet
            for (OP_IDS rt_checkOp : rt_CheckOps) {
                if (rt_checkOp == pipeInsInfo.exInstr.opcode) {
                    check_rt = true;
                    break;
                }
            }

            // checking RD & also checking intial opcode is zero
            for (FUNCT_IDS rd_checkOp : rd_CheckOps) {
                if (pipeInsInfo.exInstr.opcode == 0 && pipeInsInfo.exInstr.funct == rd_checkOp) {
                    check_rd = true;
                    assert(check_rd != check_rt);
                    break;
                }
            }
            // checking branch uses
            if ((pipeInsInfo.idInstr.rs == pipeInsInfo.exInstr.rt || pipeInsInfo.idInstr.rt == pipeInsInfo.exInstr.rt) && 
                (pipeInsInfo.idInstr.opcode == OP_BEQ || pipeInsInfo.idInstr.opcode == OP_BNE) && check_rt) {

                dumpPipeState(pipeState, output);
                pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                pipeState.exInstr = 0;   // ID -> EX
                cycleCount++;
                pipeState.cycle = cycleCount;
            }
            else if ((pipeInsInfo.idInstr.rs == pipeInsInfo.exInstr.rd || pipeInsInfo.idInstr.rt == pipeInsInfo.exInstr.rd) && 
                (pipeInsInfo.idInstr.opcode == OP_BEQ || pipeInsInfo.idInstr.opcode == OP_BNE) && check_rd) {

                dumpPipeState(pipeState, output);
                pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                pipeState.exInstr = 0;   // ID -> EX
                cycleCount++;
                pipeState.cycle = cycleCount;
            }
            else if ((pipeInsInfo.idInstr.rs == pipeInsInfo.exInstr.rt) && 
                (pipeInsInfo.idInstr.opcode == OP_BGTZ || pipeInsInfo.idInstr.opcode == OP_BLEZ) && check_rt) {
                cout << "Branch found " << endl;
                dumpPipeState(pipeState, output);

                pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                pipeState.exInstr = 0;   // ID -> EX
            
                cycleCount++;
                pipeState.cycle = cycleCount;
            }
            else if ((pipeInsInfo.idInstr.rs == pipeInsInfo.exInstr.rd) && 
                (pipeInsInfo.idInstr.opcode == OP_BGTZ || pipeInsInfo.idInstr.opcode == OP_BLEZ) && check_rd) {
                cout << "Branch found " << endl;
                dumpPipeState(pipeState, output);

                pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                pipeState.exInstr = 0;   // ID -> EX
            
                cycleCount++;
                pipeState.cycle = cycleCount;
            }

        }
        

        while (iCacheDelay + dCacheDelay > 0) {
            // cout << "iCache: " << iCacheDelay << endl;
            // cout << "dCache: " << dCacheDelay << endl;

            dumpPipeState(pipeState, output);
            cycleCount++;
            count++;
            pipeState.cycle = cycleCount;
            
            
            if (iCacheDelay > 0 && dCacheDelay > 0){
                iCacheDelay--;
                dCacheDelay--;
                pipeState.wbInstr = 0;
                pipeInsInfo.wbInstr = Emulator::InstructionInfo(); // nop
                
            }
            else if (dCacheDelay > 0){

                pipeState.wbInstr = 0;
                pipeInsInfo.wbInstr = Emulator::InstructionInfo(); // nop
                dCacheDelay--;

            }
            else if (iCacheDelay){
                pipeState.wbInstr = pipeState.memInstr;  // MEM -> WB
                pipeState.memInstr = pipeState.exInstr;  // EX -> MEM
                pipeState.exInstr = pipeState.idInstr;   // ID -> EX
                pipeState.idInstr = 0;
                
                pipeInsInfo.wbInstr = pipeInsInfo.memInstr;
                pipeInsInfo.memInstr = pipeInsInfo.exInstr;
                pipeInsInfo.exInstr = pipeInsInfo.idInstr;
                pipeInsInfo.idInstr = Emulator::InstructionInfo();
                iCacheDelay--;
                // since we put a new instruction into mem, we need to check whether it produces a D cache miss
                if (pipeInsInfo.memInstr.isValid && (pipeInsInfo.memInstr.opcode == OP_LBU || pipeInsInfo.memInstr.opcode == OP_LHU || pipeInsInfo.memInstr.opcode == OP_LW)){
                    dCacheDelay = dCache->access(pipeInsInfo.memInstr.loadAddress, CACHE_READ) ? 0 : dCache->config.missLatency;
                }

                if (pipeInsInfo.memInstr.isValid && (pipeInsInfo.memInstr.opcode == OP_SB || pipeInsInfo.memInstr.opcode == OP_SH || pipeInsInfo.memInstr.opcode == OP_SW)){
                    dCacheDelay = dCache->access(pipeInsInfo.memInstr.storeAddress, CACHE_WRITE) ? 0 : dCache->config.missLatency;
                }
            }
        }
        //iCacheDelay = 0;
        //dCacheDelay = 0;
        //pipeState.cycle = cycleCount;

        
        // halting on first entry in IF -> should finish WB and then dumpPipeState
        if (info.isHalt) {
            status = HALT;
            // flush all stages 
            for (uint32_t i = 0; i < 5; i++) {
                // Shift pipeline stages with no new instruction in IF
                cycleCount++;
                dumpPipeState(pipeState, output);  // Log state for each flush cycle
                pipeState.wbInstr = pipeState.memInstr;
                pipeState.memInstr = pipeState.exInstr;
                pipeState.exInstr = pipeState.idInstr;
                pipeState.idInstr = pipeState.ifInstr;
                pipeState.cycle = cycleCount;
                pipeState.ifInstr = 0;  // No new instruction to fetch
                // cout << pipeState.wbInstr;
            }
            break;
        } else {
            count ++;
            cycleCount++;
        }
        dumpPipeState(pipeState, output);
    }

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
