# edit the emulator.h to make uint32_t PC; public (move into public section)
# add this to cycle.cpp
# if(!info.isHalt && cycleCount > 5) {
#            info.isValid = false;
#            emulator->PC = 0x8000;
#        }

.set noreorder

lw $t0, 0($t0)
add $t0, $t0, $t0
.word 0xfeedfeed  # Place the value at 0x8000
