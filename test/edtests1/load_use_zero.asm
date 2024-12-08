.set noreorder

# Load-use stall with $zero (should not stall)
main:  la   $t0, LoadAddr        # Load address of load
       lw   $zero, 0($t0)         # Attempt to load into $zero (no effect)
       add  $t1, $zero, $t2       # Uses $zero -> should not cause a stall
       .word 0xfeedfeed
LoadAddr: .word 0x6000             # Dummy data