#include "cache.h"
#include "iostream"

using namespace std;

// Tests the cache using 375 PSET 5. It should miss everything
int main() {

    CacheConfig config = {
        .cacheSize = 1,
        .blockSize = 1,
        .ways = 1,
        .missLatency = 1,
    };

    CacheDataType cacheType = I_CACHE;

    Cache cache = Cache(config, cacheType);

    cout << "Testing basic functionality... " << endl;
    cout << "Write 0: " << cache.access(0, CACHE_WRITE) << endl;
    cout << "Read 0: " << cache.access(0, CACHE_READ) << endl;
    cout << "Hits: " << cache.getHits() << endl;
    cout << "Misses: " << cache.getMisses() << endl;
}
