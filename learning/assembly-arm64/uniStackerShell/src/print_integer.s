.bss
buffer: .space 20

.text
.global print_integer

print_integer:
    stp x29, x30, [sp, #-16]! // prologue
    stp x0, x1, [sp, #-16]!

    // Convert //
    mov x1, x0 // The integer to print
    ldr x2, =buffer // Array buffer
    mov x3, x2 // Current write position
    mov x4, #0 // Digit count

    cmp x0, #0
    beq _handle_zero

    bl _convert_loop

    // Reverse //
    mov x5, #0  // Index for loop
    sub x6, x4, #1 // len-1 of number
    bl _rev_loop

    // Print //
    b _print

_end:
    ldp x0, x1, [sp], #16
    ldp x29, x30, [sp], #16
    ret

_print:
    mov x1, x2
    mov x2, x4
    bl _write
    b _end

_handle_zero:
    mov w5, #'\n'
    strb w5, [x3], #1
    add x4, x4, #1
    b _append_newline

_convert_loop:
    mov x10, #10 // temp
    udiv x5, x1, x10
    msub x6, x5, x10, x1 // x6 (mod) = x1 - (x5 * 10)
    add x6, x6, #'0'
    strb w6, [x3], #1
    mov x1, x5
    add x4, x4, #1
    cbnz x1, _convert_loop // if x1 != 0: continue
    ret

_rev_loop:
    cmp x5, x6
    bge _append_newline
    ldrb w7, [x2, x5]
    ldrb w8, [x2, x6]
    strb w8, [x2, x5]
    strb w7, [x2, x6]
    add x5, x5, #1
    sub x6, x6, #1
    b _rev_loop

_append_newline:
    mov w5, #'\n'
    add x6, x2, x4 // After the last digit
    strb w5, [x6]
    add x4, x4, #1 // Increment count
    b _print

_write:
    mov x0, #1 // stdout
    mov x8, #64 // syscall: write
    svc #0 // make syscall
    ret
