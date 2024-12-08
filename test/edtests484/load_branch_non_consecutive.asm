# Load-branch stall with an independent instruction in between
main:  lw   $t1, 0($t0)           # Load from LoadAddr into $t1
       addi $t2, $t2, 1           # Independent instruction
       beq  $t1, $t2, main          # Dependent on $t1  
.word 0xfeedfeed
