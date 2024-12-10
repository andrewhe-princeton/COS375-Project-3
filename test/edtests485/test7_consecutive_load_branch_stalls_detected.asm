# test7_consecutive_load_branch_stalls_detected.asm
# This program tests if the simulator detects two load-use stalls (in its statistics) when a branch depends on two consecutive loads.

main:
    la   $t0, Arr          # Load base address of the array
    lw   $t1, 0($t0)       # Load first value from array into $t1
    lw   $t2, 4($t0)       # Load second value from array into $t2
    beq  $t1, $t2, branch_taken  # Branch if $t1 == $t2 (dependent on both loads)
    add  $t3, $t1, $t2      # Instruction executed if branch not taken

branch_taken:
    sub  $t4, $t1, $t2      # Instruction executed if branch is taken
    .word 0xfeedfeed        # End of program

Arr: .word 0x1, 0x1         # Array with two identical values
