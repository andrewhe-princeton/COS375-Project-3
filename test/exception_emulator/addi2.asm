# –2147483648 ~ 2147483647 
# testing overflow condition for negative result
lw $t0, Arr($zero)          # t0 = –2147483648
addi $t2, $t0, -1
add $t3, $t0, $zero
Arr:  .word -0x80000000
.align 15  # Align to a 32,768-byte boundary (address 0x8000)
.word 0xfeedfeed  # Place the value at 0x8000
