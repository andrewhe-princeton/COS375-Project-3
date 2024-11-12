#include "Utilities.h"

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#define NUM_REGS 32

using namespace std;

// Printing code...
enum OP_IDS {
    // R-type opcodes...
    OP_ZERO = 0,
      // I-type opcodes...
    OP_ADDI  = 0x8,
    OP_ADDIU = 0x9,
    OP_ANDI  = 0xc,
    OP_BEQ   = 0x4,
    OP_BNE   = 0x5,
    OP_LBU   = 0x24,
    OP_LHU   = 0x25,
    OP_LL    = 0x30,
    OP_LUI   = 0xf,
    OP_LW    = 0x23,
    OP_ORI   = 0xd,
    OP_SLTI  = 0xa,
    OP_SLTIU = 0xb,
    OP_SB    = 0x28,
    OP_SC    = 0x38,
    OP_SH    = 0x29,
    OP_SW    = 0x2b,
    OP_BLEZ  = 0x6,    // blez
    OP_BGTZ  = 0x7,    // bgtz
      // J-type opcodes...
    OP_J     = 0x2,
    OP_JAL   = 0x3
};

enum FUN_IDS {
    FUN_ADD  = 0x20,
    FUN_ADDU = 0x21,
    FUN_AND  = 0x24,
    FUN_JR   = 0x08,
    FUN_NOR  = 0x27,
    FUN_OR   = 0x25,
    FUN_SLT  = 0x2a,
    FUN_SLTU = 0x2b,
    FUN_SLL  = 0x00,
    FUN_SRL  = 0x02,
    FUN_SUB  = 0x22,
    FUN_SUBU = 0x23
};

static const string regNames[NUM_REGS] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",
    "$t3",   "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5",
    "$s6",   "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
};

// Byte's the smallest thing that can hold the opcode...
static uint8_t getOpcode(uint32_t instr) { return (uint8_t)((instr >> 26) & 0x3f); }

static string getFunString(uint8_t funct) {
    switch (funct) {
        case FUN_ADD:
            return "add";
        case FUN_ADDU:
            return "addu";
        case FUN_AND:
            return "and";
        case FUN_JR:
            return "jr";
        case FUN_NOR:
            return "nor";
        case FUN_OR:
            return "or";
        case FUN_SLT:
            return "slt";
        case FUN_SLTU:
            return "sltu";
        case FUN_SLL:
            return "sll";
        case FUN_SRL:
            return "srl";
        case FUN_SUB:
            return "sub";
        case FUN_SUBU:
            return "subu";
        default:
            return "ILLEGAL";
    }
}

static void handleOpZeroInst(uint32_t instr, ostream &out_stream) {
    uint8_t rs = (instr >> 21) & 0x1f;
    uint8_t rt = (instr >> 16) & 0x1f;
    uint8_t rd = (instr >> 11) & 0x1f;
    uint8_t shamt = (instr >> 6) & 0x1f;
    uint8_t funct = instr & 0x3f;

    string funName = getFunString(funct);

    ostringstream sb;

    if (funct == FUN_JR) {
        sb << " " << funName << " " << regNames[rs] << " ";
    } else if (funct == FUN_SLL || funct == FUN_SRL) {
        if (instr == 0x0) {
            sb << " nop ";
        } else {
            sb << " " << funName << " " << regNames[rd] << ", " << regNames[rt] << ", "
               << static_cast<uint32_t>(shamt) << " ";
        }
    } else {
        sb << " " << funName << " " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]
           << " ";
    }

    out_stream << left << setw(25) << sb.str();
}

static string getImmString(uint8_t opcode) {
    switch (opcode) {
        case OP_ADDI:
            return "addi";
        case OP_ADDIU:
            return "addiu";
        case OP_ANDI:
            return "andi";
        case OP_BEQ:
            return "beq";
        case OP_BNE:
            return "bne";
        case OP_LBU:
            return "lbu";
        case OP_LHU:
            return "lhu";
        case OP_LL:
            return "ll";
        case OP_LUI:
            return "lui";
        case OP_LW:
            return "lw";
        case OP_ORI:
            return "ori";
        case OP_SLTI:
            return "slti";
        case OP_SLTIU:
            return "sltiu";
        case OP_SB:
            return "sb";
        case OP_SC:
            return "sc";
        case OP_SH:
            return "sh";
        case OP_SW:
            return "sw";
        case OP_BLEZ:
            return "blez";
        case OP_BGTZ:
            return "bgtz";
        default:
            // This should never happen.
            return "ILLEGAL";
    }
}

static void handleImmInst(uint32_t instr, ostream &out_stream) {
    uint8_t opcode = (instr >> 26) & 0x3f;
    uint8_t rs = (instr >> 21) & 0x1f;
    uint8_t rt = (instr >> 16) & 0x1f;
    uint16_t imm = instr & 0xffff;

    string opString = getImmString(opcode);

    ostringstream sb;

    switch (opcode) {
        case OP_ADDI:
        case OP_ADDIU:
        case OP_ANDI:
        case OP_ORI:
        case OP_SLTI:
        case OP_SLTIU:
            sb << " " << opString << " " << regNames[rt] << ", " << regNames[rs] << ", " << hex
               << "0x" << static_cast<uint32_t>(imm) << " ";
            break;
        case OP_BEQ:
        case OP_BNE:
            sb << " " << opString << " " << regNames[rs] << ", " << regNames[rt] << ", " << hex
               << "0x" << static_cast<uint32_t>(imm) << " ";
            break;
        case OP_LBU:
        case OP_LHU:
        case OP_LL:
        case OP_LW:
        case OP_SB:
        case OP_SC:
        case OP_SH:
        case OP_SW:
            sb << " " << opString << " " << regNames[rt] << ", " << static_cast<int16_t>(imm) << "("
               << regNames[rs] << ") ";
            break;
        case OP_LUI:
            sb << " " << opString << " " << regNames[rt] << ", " << hex << "0x"
               << static_cast<uint32_t>(imm) << " ";
            break;
        case OP_BLEZ:
        case OP_BGTZ:
            sb << " " << opString << " " << regNames[rs] << ", " << hex << "0x"
               << static_cast<uint32_t>(imm) << " ";
            break;
        default:
            // This should never happen.
            sb << "ILLEGAL";
            break;
    }

    out_stream << left << setw(25) << sb.str();
}

static void handleJInst(uint32_t instr, ostream &out_stream) {
    uint8_t opcode = (instr >> 26) & 0x3f;
    uint32_t addr = instr & 0x3ffffff;

    ostringstream sb;

    switch (opcode) {
        case OP_JAL:
            sb << " jal " << hex << "0x" << addr << " ";
            break;
        case OP_J:
            sb << " j " << hex << "0x" << addr << " ";
            break;
    }

    out_stream << left << setw(25) << sb.str();
}

static void printInstr(uint32_t curInst, ostream &pipeState) {
    if (curInst == 0xfeedfeed) {
        pipeState << left << setw(25) << " HALT ";
        return;
    } else if (curInst == 0xdeefdeef) {
        pipeState << left << setw(25) << " UNKNOWN ";
        return;
    }

    switch (getOpcode(curInst)) {
        // Everything with a zero opcode...
        case OP_ZERO:
            handleOpZeroInst(curInst, pipeState);
            break;
        case OP_ADDI:
        case OP_ADDIU:
        case OP_ANDI:
        case OP_BEQ:
        case OP_BNE:
        case OP_LBU:
        case OP_LHU:
        case OP_LL:
        case OP_LUI:
        case OP_LW:
        case OP_ORI:
        case OP_SLTI:
        case OP_SLTIU:
        case OP_SB:
        case OP_SC:
        case OP_SH:
        case OP_SW:
        case OP_BLEZ:
        case OP_BGTZ:
            handleImmInst(curInst, pipeState);
            break;
        case OP_J:
        case OP_JAL:
            handleJInst(curInst, pipeState);
            break;
        default:
            // Illegal instruction. Trigger an exception.
            // Note: Since we catch illegal instructions here, the "handle"
            // instructions don't need to check for illegal instructions.
            // except for the case with a 0 opcode and illegal function.
            pipeState << left << setw(25) << " ILLEGAL ";
    }
}

Status dumpPipeState(PipeState &state, const std::string &base_output_name) {
    static auto fileInit = false;
    auto fileOp = ios::app;
    if (!fileInit) {
        fileOp = ios::out;
        fileInit = true;
    }
    ofstream pipe_out(base_output_name + "_pipe_state.out", fileOp);

    if (pipe_out) {
        pipe_out << "Cycle: " << std::setw(8) << state.cycle << "\t|";
        pipe_out << "|";
        printInstr(state.ifInstr, pipe_out);
        pipe_out << "|";
        printInstr(state.idInstr, pipe_out);
        pipe_out << "|";
        printInstr(state.exInstr, pipe_out);
        pipe_out << "|";
        printInstr(state.memInstr, pipe_out);
        pipe_out << "|";
        printInstr(state.wbInstr, pipe_out);
        pipe_out << "|" << endl;
        return SUCCESS;
    } else {
        cerr << LOG_ERROR << "Could not open pipe state file!" << endl;
        return ERROR;
    }
}

Status dumpSimStats(SimulationStats &stats, const std::string &base_output_name) {
    ofstream simStats(base_output_name + "_sim_stats.out");

    if (simStats) {
        simStats << left << setw(23) << "Dynamic instructions: "<< stats .dynamicInstructions << endl;
        simStats << left << setw(23) << "Total cycles: "        << stats .totalCycles << endl;
        simStats << left << setw(23) << "I-cache hits: "        << stats .icHits << endl;
        simStats << left << setw(23) << "I-cache misses: "      << stats .icMisses << endl;
        simStats << left << setw(23) << "D-cache hits: "        << stats .dcHits << endl;
        simStats << left << setw(23) << "D-cache misses: "      << stats .dcMisses << endl;
	simStats << left << setw(23) << "Load-use stalls: "     << stats .loadStalls << endl;
        return SUCCESS;
    } else {
        cerr << LOG_ERROR << "Could not open sim stats file!" << endl;
        return ERROR;
    }
}
