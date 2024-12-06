#include "cache.h"
#include "iostream"
#include <cassert>

using namespace std;

// Tests 375 PSET #5 Question 1 (result should match answer key)
int main() {

    cout << "Testing PSET #5 Question 1!" << endl;

    // C = A * S * B. A is 1, S is 8, B is 1
    CacheConfig config = {
        .cacheSize = 8,
        .blockSize = 1,
        .ways = 1,
        .missLatency = 1,
    };

    CacheDataType cacheType = I_CACHE;

    Cache cache = Cache(config, cacheType);

    assert(!cache.access(3, CACHE_READ));
    assert(!cache.access(180, CACHE_READ));
    assert(!cache.access(43, CACHE_READ));
    assert(cache.access(2, CACHE_READ));
    assert(!cache.access(191, CACHE_READ));
    assert(!cache.access(88, CACHE_READ));
    assert(cache.access(190, CACHE_READ));
    assert(!cache.access(14, CACHE_READ));
    assert(cache.access(181, CACHE_READ));
    assert(!cache.access(44, CACHE_READ));
    assert(!cache.access(186, CACHE_READ));
    assert(!cache.access(253, CACHE_READ));

    assert(cache.getMisses() == 9);
    assert(cache.getHits() == 3);

    cout << "Hits: " << cache.getHits() << endl;
    cout << "Misses: " << cache.getMisses() << endl;

    cout << "Success..." << endl;
}
