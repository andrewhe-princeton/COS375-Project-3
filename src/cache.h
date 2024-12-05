#pragma once
#include <inttypes.h>

#include <iostream>
#include <vector>

#include "Utilities.h"

struct CacheConfig {
    // Cache size in bytes.
    uint32_t cacheSize;
    // Cache block size in bytes.
    uint32_t blockSize;
    // Type of cache - direct-mapped or two-way set-assoc?
    uint32_t ways;
    // Additional miss latency in cycles.
    uint32_t missLatency;
    // debug: Overload << operator to allow easy printing of CacheConfig
    friend std::ostream& operator<<(std::ostream& os, const CacheConfig& config) {
        os << "CacheConfig { " << config.cacheSize << ", " << config.blockSize << ", "
           << config.ways << ", " << config.missLatency << " }";
        return os;
    }
};

enum CacheDataType { I_CACHE = false, D_CACHE = true };
enum CacheOperation { CACHE_READ = false, CACHE_WRITE = true };

class Cache {
   private:
    /**TODO[students] include other states, e.g. associativity, cache tables */
    uint32_t hits, misses;
    uint32_t numSets;
    uint32_t blockSize;
    uint32_t numWays;

    uint32_t numTagBits;
    uint32_t numIdxBits;
    uint32_t numBlkOffsetBits;
    // 0 (LRU) <=  lru[i][j] <= ways-1 (MRU)
    std::vector<std::vector<uint32_t>> lru;
    std::vector<std::vector<uint32_t>> valid;
    std::vector<std::vector<uint32_t>> tag;

    void updateLRU(uint32_t idx, uint32_t way);
    uint32_t findLRU(uint32_t idx);

   public:
    CacheConfig config;
    // Constructor to initialize the cache parameters
    Cache(CacheConfig configParam, CacheDataType cacheType);

    /** Access methods for reading/writing
     * @return true for hit and false for miss
     * @param
     *      address: memory address
     *      readWrite: true for read operation and false for write operation
     */
    bool access(uint32_t address, CacheOperation readWrite);

    // dump information as you needed, write your own dump function
    Status dump(const std::string& base_output_name);

    uint32_t getHits() { return hits; }
    uint32_t getMisses() { return misses; }
};
