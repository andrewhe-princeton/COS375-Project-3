# arithmetic_branch_stall.asm
# This program causes a one-cycle stall due to an arithmetic-branch dependency.

main:
    li   $t0, 5            # Load immediate into $t0
    addi $t1, $t0, 10      # $t1 = $t0 + 10
    beq  $t1, $t0, skip    # Branch if $t1 == $t0 {stall one cycle when this gets to ID}
    add  $t2, $t1, $t0     # Delay slot instruction (will not execute if branch taken)
skip:
    sub  $t3, $t0, $t2     # Subtract $t2 from $t0
    .word 0xfeedfeed       # End of program
