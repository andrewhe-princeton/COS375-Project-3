#pragma once

#include <string>

#include "MemoryStore.h"
#include "RegisterInfo.h"

// Enum for opcode values
enum OP_IDS {
    // R-type opcodes...
    OP_ZERO  = 0x0,  // zero
    // I-type opcodes...
    OP_ADDI  = 0x8,  // addi
    OP_ADDIU = 0x9,  // addiu
    OP_ANDI  = 0xc,  // andi
    OP_BEQ   = 0x4,  // beq
    OP_BNE   = 0x5,  // bne
    OP_LBU   = 0x24, // lbu
    OP_LHU   = 0x25, // lhu
    OP_LUI   = 0xf,  // lui
    OP_LW    = 0x23, // lw
    OP_ORI   = 0xd,  // ori
    OP_SLTI  = 0xa,  // slti
    OP_SLTIU = 0xb,  // sltiu
    OP_SB    = 0x28, // sb
    OP_SH    = 0x29, // sh
    OP_SW    = 0x2b, // sw
    OP_BLEZ  = 0x6,  // blez
    OP_BGTZ  = 0x7,  // bgtz
    // J-type opcodes...
    OP_J     = 0x2,  // j
    OP_JAL   = 0x3,  // jal
};

// Enum for funct_IDs
enum FUNCT_IDS {
    FUN_ADD  = 0x20,   // add
    FUN_ADDU = 0x21,   // add unsigned (addu)
    FUN_AND  = 0x24,   // and
    FUN_JR   = 0x08,   // jump register (jr)
    FUN_NOR  = 0x27,   // nor
    FUN_OR   = 0x25,   // or
    FUN_SLT  = 0x2a,   // set less than (slt)
    FUN_SLTU = 0x2b,   // set less than unsigned (sltu)
    FUN_SLL  = 0x00,   // shift left logical (sll)
    FUN_SRL  = 0x02,   // shift right logical (srl)
    FUN_SUB  = 0x22,   // substract (sub)
    FUN_SUBU = 0x23    // substract unsigned (subu)
};

class Emulator {
   private:
    union REGS {
        RegisterInfo reg;
        uint32_t registers[32]{0};
    };

    // Registers
    union REGS regData;
    // memory component
    MemoryStore* memory;

    // Arch states and statistics
    uint32_t PC;
    bool encounteredBranch;
    uint32_t savedBranch;
    uint32_t din;  // Dynamic instruction number

    // Helper function to extract specific bits [start, end] from a 32-bit instruction
    uint extractBits(uint32_t instruction, int start, int end);

    // Helper function to sign extend a 16-bit integer to a 32-bit unsigned integer
    uint32_t signExt(uint16_t smol);

   public:
    Emulator();
    ~Emulator();

struct InstructionInfo {
        uint32_t pc = 0;             // pc
        uint32_t nextPC = 0;         // next pc after this instruction
        bool     isHalt = false;     // is this 0xfeedfeed
        bool     isValid = true;     // is this a valid instruction
        bool     isOverflow = false; // causes overflow
        uint32_t instructionID = 0;  // din of the instruction
        uint32_t instruction = 0;    // raw instruction
        uint32_t opcode = 0;         // bit-fields for this instruction
        uint32_t rs = 0;
        uint32_t rt = 0;
        uint32_t rd = 0;
        uint32_t shamt = 0;
        uint32_t funct = 0;
        uint16_t immediate = 0;
        uint32_t address = 0;
        int32_t  signExtImm = 0;
        uint32_t zeroExtImm = 0;
        uint32_t branchAddr = 0;
        uint32_t jumpAddr = 0;
        uint32_t loadAddress = 0;  // load and store addresses for instruction
        uint32_t storeAddress = 0; 
    };

    // getters and setters
    auto getPC() { return PC; }
    auto getDin() { return din; }
    auto getMemory() { return memory; }

    void setMemory(MemoryStore* mem) { memory = mem; }

    // functionally execute one instruction
    InstructionInfo executeInstruction();

    // Helper function to dump registers and memory
    void dumpRegMem(const std::string& output_name);
};
