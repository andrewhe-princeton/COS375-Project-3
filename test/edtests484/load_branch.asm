# Load-branch stall with an independent instruction in between
main:  lw   $t1, 0($t0)           # Load from LoadAddr into $t1
       lw   $t2, 4($t0) 
       beq  $t1, $t2, main          # Dependent on $t1  
.word 0xfeedfeed
