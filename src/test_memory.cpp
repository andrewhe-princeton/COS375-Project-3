#include <iostream>

#include "Utilities.h"
#include "MemoryStore.h"

using namespace std;


/**
 * @brief This is just a sample test file to test the memory store, you are free to modify it or add other tests as you wish.
 * You are also very welcome to share your test cases with other students on Ed.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char** argv) {
    std::cout << "Memory Test" << std::endl;

    if (argc < 2) {
        cerr << LOG_ERROR << "Usage: " << argv[0] << " <input_file>" << endl;
        return ERROR;
    }

    cout << "[Simulator] Loading memory from " << LOG_VAR(argv[1]) << endl;

    auto mem = new MemoryStore(0, MEMORY_SIZE, argv[1]);

    for (int i = 0; i < MEMORY_SIZE; i+=WORD_SIZE) {
        uint32_t value;
        mem->getMemValue(i, value, WORD_SIZE);
        if (value != 0) cout << hex << "Memory[" << i << "] = " << value << endl;
    }

    return 0;
}