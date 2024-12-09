# invalid_instruction_exception_test.asm
# This program triggers an illegal instruction exception and tests the exception handler.

main:
    li   $t0, 42             # Load a valid value into $t0 (setup)
    .word 0xFFFFFFFF         # Insert an illegal instruction (invalid opcode)
    .word 0xfeedfeed         # End of program (should be skipped if exception occurs)

    # Align the exception handler at address 0x8000
    .align 12                # Align to a 0x1000-byte boundary
    .org 0x8000              # Start placing the subsequent instructions at 0x8000

exception_handler:
    li   $t1, 0xDEADBEEF     # Load a value to confirm handler execution
    addi $t1, $t1, 1         # Increment to verify handler behavior
    .word 0xfeedfeed         # End of exception handler
