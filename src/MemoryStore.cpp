#include "MemoryStore.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Utilities.h"

using namespace std;

MemoryStore::MemoryStore(uint32_t startAddr, uint32_t numEntries)
    : startAddr(startAddr) {
    // fill memory array with 0s
    memArr.resize(numEntries);
    std::fill(memArr.begin(), memArr.end(), 0);

    // If we can't initialise memory appropriately, don't return a
    // MemoryStore at all.
    assert((prepareMemory(this) == 0));
}

MemoryStore::MemoryStore(uint32_t startAddr, uint32_t numEntries, const char *fileName)
    : startAddr(startAddr) {
    // fill memory array with 0s
    memArr.resize(numEntries);
    std::fill(memArr.begin(), memArr.end(), 0);

    // If we can't initialise memory appropriately, don't return a
    // MemoryStore at all.
    assert((prepareMemory(this) == 0));

    loadFromFile(fileName);
}

int prepareMemory(MemoryStore *mem) {
    ifstream initMem;
    initMem.open("init_mem_image", ios::in);

    // For tests that don't require such an initial memory image, nothing is done.
    while (initMem && mem) {
        uint32_t curVal = 0;
        uint32_t addr = 0;

        initMem >> hex >> addr;
        initMem >> hex >> curVal;

        int ret = mem->setMemValue(addr, curVal, WORD_SIZE);

        if (ret) {
            cout << LOG_ERROR << "Could not set initial memory value!" << endl;
            return -EINVAL;
        }
    }

    return 0;
}

int MemoryStore::getOrSetValue(bool get, uint32_t address, uint32_t &value, MemEntrySize size) {
    uint32_t byteSize = static_cast<uint32_t>(size);

    switch (size) {
        case BYTE_SIZE:
        case HALF_SIZE:
        case WORD_SIZE:
            break;
        default:
            cerr << LOG_ERROR << "Invalid size passed, cannot read/write memory" << endl;
            return -EINVAL;
    }

    uint32_t relativeAddr = address - startAddr;
    try {
        if (get) {
            value = 0;
            for (uint32_t i = 0; i < byteSize; ++i) {
                value |= (memArr.at(relativeAddr + i) << ((byteSize - 1 - i) * 8));
            }
        } else {
            for (uint32_t i = 0; i < byteSize; ++i) {
                memArr.at(relativeAddr + i) = (value >> ((byteSize - 1 - i) * 8)) & 0xFF;
            }
        }
    } catch (const std::out_of_range &e) {
        cerr << LOG_ERROR << "Access violation at address 0x" << hex << address << endl;
        return -EINVAL;
    }

    return 0;
}

int MemoryStore::getMemValue(uint32_t address, uint32_t &value, MemEntrySize size) {
    return getOrSetValue(true, address, value, size);
}

int MemoryStore::setMemValue(uint32_t address, uint32_t value, MemEntrySize size) {
    return getOrSetValue(false, address, value, size);
}

int MemoryStore::loadFromFile(const char *fileName) {
    // Open instruction file
    ifstream infile(fileName, ios::binary | ios::in);

    if (infile) {
        // Get length of the file and read instruction file into a buffer
        infile.seekg(0, ios::end);
        int length = infile.tellg();
        infile.seekg(0, ios::beg);

        char *buf = new char[length];
        infile.read(buf, length);
        infile.close();

        // Initialize memory store with buffer contents
        int memLength = length / sizeof(buf[0]);
        int i;
        for (i = 0; i < memLength; i++) {
            this->setMemValue(i * BYTE_SIZE, buf[i], BYTE_SIZE);
        }
        return SUCCESS;
    } else {
        std::cerr << LOG_ERROR << "Unable to open memory file " << fileName << std::endl;
        return ERROR;
    }
}

int MemoryStore::printMemArray(uint32_t startAddr, uint32_t endAddr, uint32_t entrySize,
                               uint32_t entriesPerRow, std::ostream &out_stream) {
    // Validate the entry size
    switch (entrySize) {
        case BYTE_SIZE:
        case HALF_SIZE:
        case WORD_SIZE:
            break;
        default:
            cerr << LOG_ERROR << "Invalid print size passed, cannot print memory" << endl;
            return -EINVAL;
    }

    uint32_t relStart = startAddr - this->startAddr;
    uint32_t relEnd = endAddr - this->startAddr;
    uint32_t curAddr = startAddr;

    try {
        while (relStart < relEnd) {
            out_stream << "0x" << hex << setfill('0') << setw(WORD_WIDTH) << curAddr << ": ";
            for (uint32_t i = 0; i < entriesPerRow; i++) {
                if (relStart < relEnd) {
                    out_stream << "0x";
                    for (int j = 0; j < (int)(entrySize); j++) {
                        out_stream << hex << setfill('0') << setw(BYTE_WIDTH)
                                   << (uint32_t)(memArr.at(relStart + j));
                    }
                    relStart += entrySize;
                    out_stream << " ";
                } else {
                    out_stream << endl;
                    return 0;
                }
            }

            out_stream << endl;
            curAddr += (uint32_t)(entrySize)*entriesPerRow;
        }
    } catch (const std::out_of_range &e) {
        cerr << LOG_ERROR << "Access violation at address 0x" << hex << curAddr << endl;
        return -EINVAL;
    }

    return 0;
}

int MemoryStore::printMemory(uint32_t startAddr, uint32_t endAddress) {
    return printMemArray(startAddr, endAddress, WORD_SIZE, 5, std::cout);
}

void dumpMemoryState(MemoryStore *mem, const std::string &base_output_name) {
    uint32_t startAddr;
    uint32_t endAddr;
    startAddr = 0;
    endAddr = 0x1f4;  // 500

    MemoryStore *memImpl = dynamic_cast<MemoryStore *>(mem);
    if (!memImpl) {
        cerr << LOG_ERROR << "Invalid memory store passed to dump function" << endl;
    }
    ifstream memRange;
    memRange.open("print_mem_range", ios::in);
    // For tests that don't specify a memory range to print out, the default is used.
    if (memRange) {
        memRange >> hex >> startAddr;
        memRange >> hex >> endAddr;
    }
    ofstream mem_out(base_output_name + "_mem_state.out");

    if (mem_out) {
        mem_out << "---------------------" << endl;
        mem_out << "Begin Memory State" << endl;
        mem_out << "---------------------" << endl;
        memImpl->printMemArray(startAddr, endAddr, WORD_SIZE, 5, mem_out);
        mem_out << "---------------------" << endl;
        mem_out << "End Memory State" << endl;
        mem_out << "---------------------" << endl;
    } else {
        cerr << LOG_ERROR << "Could not create memory state dump file" << endl;
    }
}
