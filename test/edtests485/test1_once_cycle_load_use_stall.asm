# load_use_stall.asm
# This program creates a load-use stall situation.

main:
    la   $t0, Arr       # Load base address of the array
    lw   $t2, 0($t0)    # Load first element of the array into $t1
    sw   $t1,  0($t2)   # Use the value loaded into $t2 without waiting, stall until lw @ MEM to FWD
    add  $t2, $t1, $t1 
    sw   $t2, 4($t0)    # Store the result back into the array
    .word 0xfeedfeed

Arr: .word 0x4          # Initial array value
     .word 0x3
     .word 0x2          # Space for result