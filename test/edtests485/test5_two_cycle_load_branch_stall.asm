# load_branch_stall.asm
# This program causes a two-cycle stall due to a load-branch dependency.
# IMPOTANT NOTE: Disable IF stalls to allow 'lw $t1, 0($t0)' to be detected as a 
# load-use stall... This is a work-around to paying more attention to the i-cache
# to prevent the beq instruction from missing

main:
    la   $t0, Arr          # Load base address of the array
    lw   $t1, 0($t0)       # Load value from array into $t1
    bne  $t1, $zero, done  # Branch if $t1 != 0... need to FWD from mem -> ID so stall 2 cycles 
    add  $t2, $t1, $t1     # Delay slot instruction (executes even if branch taken)... should insert a nop after bne
done:
    sub  $t3, $t1, $t0     # Subtract $t0 from $t1
    .word 0xfeedfeed       # End of program

Arr: .word 0x1             # Initial value in array
