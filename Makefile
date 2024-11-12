# Makefile

# Build targets:
# make sim_cycle # build sim_cycle
# make sim_funct # build sim_funct
# make all # build sim_funct, sim_cycle and all tests
# make tests # build all assembly tests
# make clean $ removes sim_cycle, sim_funct, and all .bin and .elf files in test/

# Note: If you're having trouble getting the assembler and objcopy executables to work,
# you might need to mark those files as executables using 'chmod +x filename'

# Compiler settings
CC = g++
# Note: All builds will contain debug information
CFLAGS = --std=c++14 -Wall -g -pedantic -O2


# Source and header files
SIM_FUNCT_SRC = sim_funct.cpp funct.cpp emulator.cpp MemoryStore.cpp Utilities.cpp
SIM_CYCLE_SRC = sim_cycle.cpp cycle.cpp cache.cpp emulator.cpp MemoryStore.cpp Utilities.cpp
SIM_FUNCT_SRCS = $(addprefix src/, $(SIM_FUNCT_SRC))
SIM_CYCLE_SRCS = $(addprefix src/, $(SIM_CYCLE_SRC))
COMMON_HDRS = $(wildcard src/*.h)

ASSEMBLY_TESTS = $(wildcard test/*.asm)
ASSEMBLY_TARGETS = $(ASSEMBLY_TESTS:.asm=.bin)

ASSEMBLER = bin/mips-linux-gnu-as
OBJCOPY = bin/mips-linux-gnu-objcopy

# Main targets
all: sim_funct sim_cycle tests

sim_funct: $(SIM_FUNCT_SRCS) $(COMMON_HDRS)
	$(CC) $(CFLAGS) -o sim_funct $(SIM_FUNCT_SRCS)

sim_cycle: $(SIM_CYCLE_SRCS) $(COMMON_HDRS)
	$(CC) $(CFLAGS) -o sim_cycle $(SIM_CYCLE_SRCS)

# Test targets
tests: $(ASSEMBLY_TARGETS)

$(ASSEMBLY_TARGETS) : test/%.bin : test/%.asm
	$(ASSEMBLER) test/$*.asm -o test/$*.elf
	$(OBJCOPY) test/$*.elf -j .text -O binary test/$*.bin

# Clean function
clean:
	rm -f sim_funct sim_cycle
	rm -f test/*.bin test/*.elf

# Phony targets
.PHONY: all debug tests clean
