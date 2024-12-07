# Illegal instruction test
main: .word 0xFFFFFFFF           # Undefined instruction encoding (e.g., OP_ILLEGAL)
      add $t0, $t0, $t0
      addi $t1, $t1, 0
      .word 0xfeedfeed
      