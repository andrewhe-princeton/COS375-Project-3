#pragma once
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// Utilities macro, they are very useful for debugging
// Check sim_cycle.cpp to see how to use them!
#define LOG_INFO \
    "[INFO] (" << __FILE__ << ":" << __LINE__ << "): "
#define LOG_ERROR \
    "[ERROR] (" << __FILE__ << ":" << __LINE__ << "): "
#define LOG_VAR(var) \
    #var << ": " << var << " "


enum Status { SUCCESS = 0, ERROR = 1, HALT = 2 };

struct PipeState {
    uint32_t cycle;
    uint32_t ifInstr;
    uint32_t idInstr;
    uint32_t exInstr;
    uint32_t memInstr;
    uint32_t wbInstr;
};

struct SimulationStats {
    uint32_t dynamicInstructions;
    uint32_t totalCycles;
    uint32_t icHits;
    uint32_t icMisses;
    uint32_t dcHits;
    uint32_t dcMisses;
// NOTE: loadStalls tracks both load-arithmetic and load-branch stalls
    uint32_t loadStalls;
};

// Implemented in UtilityFunctions.o
Status dumpPipeState(PipeState& state, const std::string& base_output_name);
Status dumpSimStats(SimulationStats& stats, const std::string& base_output_name);

// Endian Helpers
inline uint32_t ConvertWordToBigEndian(uint32_t value) { return htonl(value); }
inline uint16_t ConvertHalfWordToBigEndian(uint16_t value) { return htons(value); }

// handle output file names
inline std::string getBaseFilename(const char* inputPath) {
    std::string path(inputPath);
    size_t end = path.rfind('.');

    if (end != std::string::npos) {
        return path.substr(0, end);
    } else {
        return path;
    }
}
