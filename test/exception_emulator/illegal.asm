# edit the emulator.h to make uint32_t PC; public (move into public section)
# add this to cycle.cpp
# if(!info.isHalt && cycleCount > 5) {
#            info.isValid = false;
#            emulator->PC = 0x8000;
#        }
addi $t0, $t0, 5
add $t0, $t0, $t0
.align 15  # Align to a 32,768-byte boundary (address 0x8000)
.word 0xfeedfeed  # Place the value at 0x8000
