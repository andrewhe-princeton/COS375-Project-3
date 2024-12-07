.set noreorder

lw $t0, 0($t1)
add $t2, $t0, $t3
beq $t0, $t2, test
test: 
    addi $t5, $t5, 5
.word 0xfeedfeed  # Place the value at 0x8000
