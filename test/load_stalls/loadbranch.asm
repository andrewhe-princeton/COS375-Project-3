.set noreorder

addi $t2, $t2, 3
lw $t0, 0($t1)
bgtz $t0, jlabel
jlabel: addi $t2, $t2, 3
.word 0xfeedfeed  # Place the value at 0x8000
