# –2147483648 ~ 2147483647 
# testing overflow condition for positive result
lw $t0, Arr($zero)         # t0 = 2147483647
addi $t1, $zero, 1
add $t2, $t0, $t1
add $t3, $t0, $zero
Arr:  .word 0x7FFFFFFF             
.align 15  # Align to a 32,768-byte boundary (address 0x8000)
add $t0, $t0, $t0 # begins fetching from 0x8000 and after
.word 0xfeedfeed  # Place the value at 0x8000
