.extern print_integer
.extern to_integer

.bss
buffer: .space 20

.data
prompt: .asciz "Enter n: "

.text
.global _start

_start:
    // Get N //
    ldr x1, =prompt
    mov x2, #9
    bl write_string

    // Reading N
    ldr x1, =buffer
    mov x2, #20
    bl read_string

    // Converting N to an integer
    bl to_integer
    mov x5, x0 // x5 = integer N

    // Start Fibbonaci loop //
    mov x3, #0 // loop index
    mov x0, #0
    mov x1, #1
    bl loop

_end:
    mov x0, #0
    mov x8, #93
    svc #0

write_string:
    mov x0, #1
    mov x8, #64
    svc #0
    ret

read_string:
    mov x0, #0
    mov x8, #63
    svc #0
    ret

loop:
    cmp x3, x5
    bgt _end
    bl print_integer

    add x2, x1, x0
    mov x0, x1
    mov x1, x2

    add x3, x3, #1
