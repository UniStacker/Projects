.section .data
buffer: .skip 20     // max 20 digits + newline

.section .text
.global print_number

print_number:
    stp x29, x30, [sp, #-16]!
    mov x29, sp

    mov x1, x0              // number to print
    ldr x2, =buffer         // buffer pointer
    mov x3, x2              // save start of buffer
    mov x4, #0              // digit count

    cmp x1, #0
    b.ne .convert

    // handle zero explicitly
    mov w5, #'0'
    strb w5, [x2], #1
    mov x4, #1
    b .done

.convert:
    mov x5, #10
.loop:
    udiv x6, x1, x5
    msub x7, x6, x5, x1     // remainder
    add x7, x7, #'0'
    strb w7, [x2], #1
    add x4, x4, #1
    mov x1, x6
    cmp x1, #0
    b.ne .loop

.done:
    // Reverse the digit string (in-place)
    mov x6, #0
    sub x7, x4, #1

.rev_loop:
    cmp x6, x7
    b.ge .append_newline
    ldrb w8, [x3, x6]
    ldrb w9, [x3, x7]
    strb w9, [x3, x6]
    strb w8, [x3, x7]
    add x6, x6, #1
    sub x7, x7, #1
    b .rev_loop

.append_newline:
    mov w5, #'\n'
    strb w5, [x3, x4]
    add x4, x4, #1 // Increment length

.print:
    mov x0, #1              // stdout
    mov x1, x3              // buffer
    mov x2, x4              // length
    mov x8, #64             // syscall: write
    svc #0

    ldp x29, x30, [sp], #16
    ret
