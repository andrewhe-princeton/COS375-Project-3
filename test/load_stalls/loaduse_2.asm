.set noreorder

lw $t0, 0($zero)
lw $t0, 0($t0)
add $t0, $t0, $t0

.word 0xfeedfeed  # Place the value at 0x8000
