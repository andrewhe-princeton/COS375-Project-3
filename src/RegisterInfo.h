#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>

#include "Utilities.h"

#define V_REG_SIZE 2
#define A_REG_SIZE 4
#define T_REG_SIZE 10
#define S_REG_SIZE 8
#define K_REG_SIZE 2

#define REG_OUTPUT_WIDTH 8

using namespace std;

// A struct that can hold the values of all architectural registers.
struct RegisterInfo {
    uint32_t zero;
    // The $at register.
    uint32_t at;
    // The $v registers.
    uint32_t v[2];
    // The $a registers.
    uint32_t a[4];
    // The $t registers.
    uint32_t t[10];
    // The $s registers.
    uint32_t s[8];
    // The $k registers.
    uint32_t k[2];
    // The $gp register.
    uint32_t gp;
    // The $sp register.
    uint32_t sp;
    // The $fp register.
    uint32_t fp;
    // The $ra register.
    uint32_t ra;
};

inline Status dumpRegisterState(RegisterInfo& reg, const std::string& base_output_name) {
    ofstream reg_out(base_output_name + "_reg_state.out");
    // dumpRegisterStateInternal(reg, reg_out);
    if (reg_out) {
        reg_out << "---------------------" << endl;
        reg_out << "Begin Register Values" << endl;
        reg_out << "---------------------" << endl;
        reg_out << "$at = "
                << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.at << endl;
        reg_out << endl;
        for (int i = 0; i < V_REG_SIZE; i++)
            reg_out << "$v" << i << " = "
                    << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.v[i] << endl;
        reg_out << endl;
        for (int i = 0; i < A_REG_SIZE; i++)
            reg_out << "$a" << i << " = "
                    << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.a[i] << endl;
        reg_out << endl;
        for (int i = 0; i < T_REG_SIZE; i++)
            reg_out << "$t" << i << " = "
                    << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.t[i] << endl;
        reg_out << endl;
        for (int i = 0; i < S_REG_SIZE; i++)
            reg_out << "$s" << i << " = "
                    << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.s[i] << endl;
        reg_out << endl;
        for (int i = 0; i < K_REG_SIZE; i++)
            reg_out << "$k" << i << " = "
                    << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.k[i] << endl;
        reg_out << endl;
        reg_out << "$gp = "
                << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.gp << endl;
        reg_out << "$sp = "
                << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.sp << endl;
        reg_out << "$fp = "
                << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.fp << endl;
        reg_out << "$ra = "
                << "0x" << hex << setfill('0') << setw(REG_OUTPUT_WIDTH) << reg.ra << endl;
        reg_out << "---------------------" << endl;
        reg_out << "End Register Values" << endl;
        reg_out << "---------------------" << endl;
        return SUCCESS;
    } else {
        cerr << "Could not create register state dump file" << endl;
        return ERROR;
    }
}
