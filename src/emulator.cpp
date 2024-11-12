//----------------------------------------------------------------------------//
// REFERENCE SOLUTION                                                         //
//----------------------------------------------------------------------------//

#include "emulator.h"

#include <stdio.h>

#include <cassert>
#include <iostream>
using namespace std;

Emulator::Emulator() {
    // Initialize member variables
    memory = nullptr;
    PC = 0;
    encounteredBranch = false;
    savedBranch = 0;
    regData.reg = {};
    din = 0;
}

Emulator::~Emulator() {
    if (memory) delete memory;
}

// extract specific bits [start, end] from a 32 bit instruction
uint Emulator::extractBits(uint32_t instruction, int start, int end) {
    int bitsToExtract = start - end + 1;
    uint32_t mask = (1 << bitsToExtract) - 1;
    uint32_t clipped = instruction >> end;
    return clipped & mask;
}

// sign extend smol to a 32 bit unsigned int
uint32_t Emulator::signExt(uint16_t smol) {
    uint32_t x = smol;
    uint32_t extension = 0xffff0000;
    return (smol & 0x8000) ? x ^ extension : x;
}

// dump registers and memory
void Emulator::dumpRegMem(const std::string& output_name) {
    assert(memory);
    dumpRegisterState(regData.reg, output_name);
    dumpMemoryState(memory, output_name);
}

Emulator::InstructionInfo Emulator::executeInstruction() {
    assert(memory);
    InstructionInfo info;  // information struct for this instruction
    info.pc = PC;          // fill PC before its updated

    uint32_t instruction;
    memory->getMemValue(PC, instruction, WORD_SIZE);
    info.instruction = instruction;

    // increment PC & reset zero register
    info.nextPC = (encounteredBranch) ? savedBranch : PC + 4;
    if (!encounteredBranch)
        PC += 4;
    else {
        PC = savedBranch;
        encounteredBranch = false;
    }
    regData.registers[0] = 0;

    info.instructionID = din;
    din += 1;

    // check for halt instruction and return immediately
    info.isHalt = (instruction == 0xfeedfeed);
    if (instruction == 0xfeedfeed) {
        return info;
    }

    // parse instruction by completing function calls to extractBits() and set operands accordingly
    uint32_t opcode = extractBits(instruction, 31, 26);
    uint32_t rs = extractBits(instruction, 25, 21);
    uint32_t rt = extractBits(instruction, 20, 16);
    uint32_t rd = extractBits(instruction, 15, 11);
    uint32_t shamt = extractBits(instruction, 10, 6);
    uint32_t funct = extractBits(instruction, 5, 0);
    uint16_t immediate = extractBits(instruction, 15, 0);
    uint32_t address = extractBits(instruction, 25, 0);

    int32_t signExtImm = signExt(immediate);
    uint32_t zeroExtImm = immediate;

    uint32_t branchAddr = signExtImm << 2;
    uint32_t jumpAddr = (PC & 0xf0000000) ^ (address << 2);  // assumes PC += 4 just happened

    // fill the bitfields in the instruction info struct
    info.opcode = opcode;
    info.rs = rs;
    info.rt = rt;
    info.rd = rd;
    info.shamt = shamt;
    info.funct = funct;
    info.immediate = immediate;
    info.address = address;
    info.signExtImm = signExtImm;
    info.zeroExtImm = zeroExtImm;
    info.branchAddr = branchAddr;
    info.jumpAddr = jumpAddr;

    switch (opcode) {
        case OP_ZERO:  // R-type instruction
            switch (funct) {
                case FUN_ADD:
                    regData.registers[rd] = regData.registers[rs] + regData.registers[rt];
                    break;
                case FUN_ADDU:
                    regData.registers[rd] = regData.registers[rs] + regData.registers[rt];
                    break;
                case FUN_AND:
                    regData.registers[rd] = regData.registers[rs] & regData.registers[rt];
                    break;
                case FUN_JR:
                    encounteredBranch = true;
                    savedBranch = regData.registers[rs];
                    break;
                case FUN_NOR:
                    regData.registers[rd] = ~(regData.registers[rs] | regData.registers[rt]);
                    break;
                case FUN_OR:
                    regData.registers[rd] = regData.registers[rs] | regData.registers[rt];
                    break;
                case FUN_SLT:
                    regData.registers[rd] =
                        (int32_t(regData.registers[rs]) < int32_t(regData.registers[rt])) ? 1 : 0;
                    break;
                case FUN_SLTU:
                    regData.registers[rd] = (regData.registers[rs] < regData.registers[rt]) ? 1 : 0;
                    break;
                case FUN_SLL:
                    regData.registers[rd] = regData.registers[rt] << shamt;
                    break;
                case FUN_SRL:
                    regData.registers[rd] = regData.registers[rt] >> shamt;
                    break;
                case FUN_SUB:
                    regData.registers[rd] = regData.registers[rs] - regData.registers[rt];
                    break;
                case FUN_SUBU:
                    regData.registers[rd] = regData.registers[rs] - regData.registers[rt];
                    break;
                default:
                    std::cerr << LOG_ERROR << "Illegal operation..." << std::endl;
                    info.isValid = false;
            }
            break;

        case OP_ADDI:
            regData.registers[rt] = regData.registers[rs] + signExtImm;
            break;
        case OP_ADDIU:
            regData.registers[rt] = regData.registers[rs] + signExtImm;
            break;
        case OP_ANDI:
            regData.registers[rt] = regData.registers[rs] & zeroExtImm;
            break;
        case OP_BEQ:
            if (regData.registers[rs] == regData.registers[rt]) {
                encounteredBranch = true;
                savedBranch = PC + branchAddr;
            }
            break;
        case OP_BNE:
            if (regData.registers[rs] != regData.registers[rt]) {
                encounteredBranch = true;
                savedBranch = PC + branchAddr;
            }
            break;
        case OP_BLEZ:
            if (regData.registers[rs] == 0 || (regData.registers[rs] & 0x80000000)) {
                encounteredBranch = true;
                savedBranch = PC + branchAddr;
            }
            break;
        case OP_BGTZ:
            if (!(regData.registers[rs] & 0x80000000) && regData.registers[rs] != 0) {
                encounteredBranch = true;
                savedBranch = PC + branchAddr;
            }
            break;
        case OP_J:
            encounteredBranch = true;
            savedBranch = jumpAddr;
            break;
        case OP_JAL:
            encounteredBranch = true;
            regData.registers[31] = PC + 4;
            savedBranch = jumpAddr;
            break;
        case OP_LBU:
            info.loadAddress = regData.registers[rs] + signExtImm;  // capture load address
            memory->getMemValue(regData.registers[rs] + signExtImm, regData.registers[rt],
                                BYTE_SIZE);
            break;
        case OP_LHU:
            info.loadAddress = regData.registers[rs] + signExtImm;  // capture load address
            memory->getMemValue(regData.registers[rs] + signExtImm, regData.registers[rt],
                                HALF_SIZE);
            break;
        case OP_LUI:
            regData.registers[rt] = zeroExtImm << 16;
            break;
        case OP_LW:
            info.loadAddress = regData.registers[rs] + signExtImm;  // capture load address
            memory->getMemValue(regData.registers[rs] + signExtImm, regData.registers[rt],
                                WORD_SIZE);
            break;
        case OP_ORI:
            regData.registers[rt] = regData.registers[rs] | zeroExtImm;
            break;
        case OP_SLTI:
            regData.registers[rt] = (int32_t(regData.registers[rs]) < int32_t(signExtImm)) ? 1 : 0;
            break;
        case OP_SLTIU:
            regData.registers[rt] = (regData.registers[rs] < uint32_t(signExtImm)) ? 1 : 0;
            break;
        case OP_SB:
            info.storeAddress = regData.registers[rs] + signExtImm;  // capture store address
            memory->setMemValue(regData.registers[rs] + signExtImm,
                                extractBits(regData.registers[rt], 7, 0), BYTE_SIZE);
            break;
        case OP_SH:
            info.storeAddress = regData.registers[rs] + signExtImm;  // capture store address
            memory->setMemValue(regData.registers[rs] + signExtImm,
                                extractBits(regData.registers[rt], 15, 0), HALF_SIZE);
            break;
        case OP_SW:
            info.storeAddress = regData.registers[rs] + signExtImm;  // capture store address
            memory->setMemValue(regData.registers[rs] + signExtImm, regData.registers[rt],
                                WORD_SIZE);
            break;
        default:
            std::cerr << LOG_ERROR << "Illegal operation..." << std::endl;
            info.isValid = false;
    }
    return info;  // return the InstructionInfo struct of the instruction just executed
}
