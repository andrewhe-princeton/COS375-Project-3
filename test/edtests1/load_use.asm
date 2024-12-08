# load use
addi $t0, $t0, 0
lw $t3, 4($t0)
lw $t1, 0($t0)  
add $t2, $t1, $t3
.word 0xfeedfeed
