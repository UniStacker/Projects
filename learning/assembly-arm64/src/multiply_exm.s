
.extern print_integer
.global _start
_start:
    // Store frame info
    stp x29, x30, [sp, #-16]! // Prologue

    // Calculate address where word will be stored
    mov x5, sp
    sub x5, x5, #8

    // Store a word in memory
    mov w6, #2
    str w6, [x5]

    // Load the word from memory
    ldr w6, [x5]

    // Multiply if word is positive
    cmp w6, #0
    bge multiply_and_store
    b print_and_exit

multiply_and_store:
    mov w7, #13
    mul w6, w6, w7
    str w6, [x5]

print_and_exit:
    ldr w0, [x5]

    # Call print_number
    bl print_integer

    # Load frame info
    # ldp x29, x30, [sp], #16 // Epilogue
    # ret

    mov x0, #0   // Exit code
    mov x8, #93  // SysCall: exit
    svc #0       // Syscall

