.bss
buff: .space 100

.data
prompt: .asciz "$ "

.text
.global _start
_start:
    // Print the prompt
    ldr x1, =prompt // Load prompt
    mov x2, #2 // Prompt length
    bl print

    // Read input
    ldr x1, =buff // Load buffer
    mov x2, #100 // Buffer length
    bl input

    // Echo input
    mov x2, x0 // Bytes read
    bl print

_end:
    mov x0, #0 // Exit code
    mov x8, #93 // Syscall: exit
    svc #0 // Make syscall

print:
    mov x0, #1 // Stdout
    mov x8, #64 // Syscall: write
    svc #0 // Make syscall
    ret

input:
    mov x0, #0 // Stdin
    mov x8, #63 // Syscall: read
    svc #0 // Make syscall
    ret
