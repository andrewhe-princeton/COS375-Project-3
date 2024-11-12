#pragma once
#include <inttypes.h>

#include <string>
#include <vector>

// The memory is 64 KB large.
#define MEMORY_SIZE 0x10000

// The various sizes at which you can manipulate the memory.
enum MemEntrySize { BYTE_SIZE = 1, HALF_SIZE = 2, WORD_SIZE = 4 };

static const uint32_t BYTE_SHIFT = 8;
static const uint32_t BYTE_WIDTH = 2;
static const uint32_t WORD_WIDTH = 8;

// A memory abstraction interface. Allows values to be set and retrieved at a number of
// different size granularities. The implementation is also capable of printing out memory
// values over a given address range.
class MemoryStore {
   private:
    uint32_t startAddr;
    std::vector<uint8_t> memArr;

    int getOrSetValue(bool get, uint32_t address, uint32_t& value, MemEntrySize size);

   public:
    MemoryStore(uint32_t startAddr, uint32_t numEntries);
    MemoryStore(uint32_t startAddr, uint32_t numEntries, const char* fileName);
    ~MemoryStore(){};

    int loadFromFile(const char* fileName);
    int getMemValue(uint32_t address, uint32_t& value, MemEntrySize size);
    int setMemValue(uint32_t address, uint32_t value, MemEntrySize size);
    int printMemory(uint32_t startAddress, uint32_t endAddress);
    int printMemArray(uint32_t startAddr, uint32_t endAddr, uint32_t entrySize,
                      uint32_t entriesPerRow, std::ostream& out_stream);
};

// Creates a memory store.
// extern MemoryStore *createMemoryStore();

// Dumps the section of memory relevant for the test.
void dumpMemoryState(MemoryStore* mem, const std::string& base_output_name);

int prepareMemory(MemoryStore* mem);
