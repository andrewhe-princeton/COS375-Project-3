#include "cache.h"
#include "iostream"
#include <cassert>

using namespace std;

// Tests 375 PSET #5 Question 2. First loop should be all misses, second loop
// should have some hits.
int main() {

    cout << "Testing PSET #5 Question 2!" << endl;

    cout << "Andrew's PSET " << endl;
    // C = A * S * B. A is 2, B is 8, S is 4. C is 64.
    CacheConfig config = {
        .cacheSize = 64,
        .blockSize = 8,
        .ways = 2,
        .missLatency = 1,
    };

    CacheDataType cacheType = I_CACHE;

    Cache cache = Cache(config, cacheType);

    cout << "Structuring associative cache to be all misses." << endl;
    for (int i = 0; i < 500; i++) {
        assert(!cache.access(0, CACHE_READ));
        assert(!cache.access(32, CACHE_READ));
        assert(!cache.access(96, CACHE_READ));

        if (i < 4)
            cout << i << " Access Success" << endl;
    }
    assert(cache.getMisses() == 1500);
    assert(cache.getHits() == 0);
    cout << "Structuring direct mapped cache to have some hits." << endl;

    // C = A * S * B. A is 1, B is 4, S is 8. C is 32.
    config = {
        .cacheSize = 64,
        .blockSize = 8,
        .ways = 1,
        .missLatency = 1,
    };

    cacheType = I_CACHE;

    cache = Cache(config, cacheType);
    // Prime cache
    assert(!cache.access(0, CACHE_WRITE));
    assert(!cache.access(32, CACHE_WRITE));
    assert(!cache.access(96, CACHE_WRITE));

    for (int i = 0; i < 500; i++) {
        assert(cache.access(0, CACHE_READ));
        assert(!cache.access(32, CACHE_READ));
        assert(!cache.access(96, CACHE_READ));
        if (i < 4)
            cout << i << " Access Success" << endl;
    }

    assert(cache.getMisses() == 1003);
    assert(cache.getHits() == 500);

    cout << "Rishabh's PSET" << endl;

    // C = A * S * B. A is 2, B is 4, S is 4. C is 32.
    config = {
        .cacheSize = 32,
        .blockSize = 4,
        .ways = 2,
        .missLatency = 1,
    };

    cacheType = I_CACHE;

    cache = Cache(config, cacheType);

    cout << "Structuring associative cache to be all misses." << endl;
    for (int i = 0; i < 500; i++) {
        assert(!cache.access(4, CACHE_READ));
        assert(!cache.access(20, CACHE_READ));
        assert(!cache.access(36, CACHE_READ));

        if (i < 4)
            cout << i << " Access Success" << endl;
    }

    assert(cache.getMisses() == 1500);
    assert(cache.getHits() == 0);

    cout << "Structuring direct mapped cache to have some hits." << endl;

    // C = A * S * B. A is 1, B is 4, S is 8. C is 32.
    config = {
        .cacheSize = 32,
        .blockSize = 4,
        .ways = 1,
        .missLatency = 1,
    };

    cacheType = I_CACHE;

    cache = Cache(config, cacheType);
    // Prime cache
    assert(!cache.access(4, CACHE_WRITE));
    assert(!cache.access(20, CACHE_WRITE));
    assert(!cache.access(36, CACHE_WRITE));

    for (int i = 0; i < 500; i++) {
        assert(!cache.access(4, CACHE_READ));
        assert(cache.access(20, CACHE_READ));
        assert(!cache.access(36, CACHE_READ));

        if (i < 4)
            cout << i << " Access Success" << endl;
    }

    assert(cache.getMisses() == 1003);
    assert(cache.getHits() == 500);
}
