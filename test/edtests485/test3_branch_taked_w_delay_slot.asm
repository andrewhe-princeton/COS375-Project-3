# branch_delay.asm
# This program tests branch handling with a delay slot.


main:
    li   $t0, 10         # Load immediate into $t0
    li   $t1, 5          # Load immediate into $t1
    beq $t1, $zero, skip       # used to keep compiler from making optimization above ^^^ d
    bgtz $t0, skip       # Branch if $t0 > 0 (taken)
    add  $t2, $t1, $t1   # Delay slot should prevent this add since branch above taken
skip:
    sub  $t3, $t0, $t1   # Subtract $t1 from $t0
    .word 0xfeedfeed     # End of program
