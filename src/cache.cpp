// Sample cache implementation with a random variable to simulate hits and misses

#include "cache.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <random>
#include <cmath>

#include "Utilities.h"
#include "emulator.h"

using namespace std;

// TODO: Modify this file to complete your implementation of the cache

// Random generator for cache hit/miss simulation
static std::mt19937 generator(42);  // Fixed seed for deterministic results
std::uniform_real_distribution<double> distribution(0.0, 1.0);

// Constructor definition
Cache::Cache(CacheConfig configParam, CacheDataType cacheType) : config(configParam) {
    // Here you can initialize other cache-specific attributes
    // For instance, if you had cache tables or other structures, initialize them here
    hits = 0;
    misses = 0;
    numSets = config.cacheSize / (config.blockSize * config.ways);
    blockSize = config.blockSize;
    numWays = config.ways;
    // 4 byts per word
    // #words in a block = blockSize/4
    numBlkOffsetBits = (uint32_t)(std::log2(blockSize/4)); 
    numIdxBits = (uint32_t)(std::log2(numSets));
    // last 2 bits for byte offset
    numTagBits = 32 - numBlkOffsetBits - numIdxBits - 2;

    // Initialize LRU, valid, and tag arrays
    lru.resize(numSets);
    valid.resize(numSets);
    tag.resize(numSets);
    for (uint32_t i = 0; i < numSets; i++) {
        lru[i].resize(numWays, 0);
        for (uint32_t j = 0; j<numWays; j++)
            lru[i][j] = j;
        valid[i].resize(numWays, 0);
        tag[i].resize(numWays, 0);
    }
}

// Access method definition
// NOTE readWrite is redundant here, ignore it Ed#376
bool Cache::access(uint32_t address, CacheOperation readWrite) {
    bool hit = false;
    uint32_t tagVal = extractBits(address, 31, 31 - numTagBits + 1);
    uint32_t idx = extractBits(address, 31 - numTagBits, 
                                        31 - numTagBits - numIdxBits + 1);
    uint32_t way;                   
    for (way = 0; way < numWays; way++) {
        if (valid[idx][way] && tag[idx][way] == tagVal) {
            hit = true;
            break;
        }
    }

    if (hit) {
        updateLRU(idx, way);
    } else {
        uint32_t way = findLRU(idx);
        valid[idx][way] = 1;
        tag[idx][way] = tagVal;
        updateLRU(idx, way);
    }

    hits += hit;
    misses += !hit;
    return hit;
}

void Cache::updateLRU(uint32_t idx, uint32_t way) {
    uint32_t oldLRU = lru[idx][way];
    for (uint32_t i = 0; i < numWays; i++) {
        if (lru[idx][i] > oldLRU) {
            lru[idx][i]--;
        }
    }
    lru[idx][way] = numWays - 1;
    assert(valid[idx][way]);
}

uint32_t Cache::findLRU(uint32_t idx) {
    for (uint32_t i = 0; i < numWays; i++) {
        if (lru[idx][i] == 0) {
            return i;
        }
    }
    assert(0);
}

// Dump method definition, you can write your own dump info
Status Cache::dump(const std::string& base_output_name) {
    ofstream cache_out(base_output_name + "_cache_state.out");
    // dumpRegisterStateInternal(reg, cache_out);
    if (cache_out) {
        cache_out << "---------------------" << endl;
        cache_out << "Begin Register Values" << endl;
        cache_out << "---------------------" << endl;
        cache_out << "Cache Configuration:" << std::endl;
        cache_out << "Size: " << config.cacheSize << " bytes" << std::endl;
        cache_out << "Block Size: " << config.blockSize << " bytes" << std::endl;
        cache_out << "Ways: " << (config.ways == 1) << std::endl;
        cache_out << "Miss Latency: " << config.missLatency << " cycles" << std::endl;
        cache_out << "---------------------" << endl;
        cache_out << "End Register Values" << endl;
        cache_out << "---------------------" << endl;
        return SUCCESS;
    } else {
        cerr << LOG_ERROR << "Could not create cache state dump file" << endl;
        return ERROR;
    }

    // Here you can also dump the state of the cache, its stats, or any other relevant information
}
