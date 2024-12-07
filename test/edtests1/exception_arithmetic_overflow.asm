# Arithmetic overflow test
main:  addi  $t0,  $t0, 0   
       addi $t0, $t0, 0x7FFFFFFF      # Load maximum positive 32-bit integer into $t0
       add  $t0, $t0, $t0        # $t1 = $t0 + $t0 -> Overflow occurs
       .word 0xfeedfeed
       