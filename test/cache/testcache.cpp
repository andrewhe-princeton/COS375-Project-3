#include "cache.h"
#include "iostream"

using namespace std;

int main() {

  CacheConfig config = {
      .cacheSize = 1,
      .blockSize = 1,
      .ways = 1,
      .missLatency = 1,
  };

  CacheDataType cacheType = I_CACHE;

  Cache cache = Cache(config, cacheType);

  cout << "Write 0: " << cache.access(0, CACHE_WRITE) << endl;
  cout << "Read 0: " << cache.access(0, CACHE_READ) << endl;
}
