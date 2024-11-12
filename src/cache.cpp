// Sample cache implementation with a random variable to simulate hits and misses

#include "cache.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <random>

#include "Utilities.h"

using namespace std;

// TODO: Modify this file to complete your implementation of the cache

// Random generator for cache hit/miss simulation
static std::mt19937 generator(42);  // Fixed seed for deterministic results
std::uniform_real_distribution<double> distribution(0.0, 1.0);

// Constructor definition
Cache::Cache(CacheConfig configParam, CacheDataType cacheType) : config(configParam) {
    // Here you can initialize other cache-specific attributes
    // For instance, if you had cache tables or other structures, initialize them here
}

// Access method definition
bool Cache::access(uint32_t address, CacheOperation readWrite) {
    // For simplicity, we're using a random boolean to simulate cache hit/miss
    bool hit = distribution(generator) < 0.20;  // random 20% hit for a strange cache
    hits += hit;
    misses += !hit;
    return hit;
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
