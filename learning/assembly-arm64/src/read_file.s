.data
name_prompt: .asciz "File Name: "
fail_msg: .asciz "Failed to open file."

.bss
buffer: .space 100

.text
.global _start
_start:
    // Get file name //

    // Print prompt
    ldr x1, =name_prompt
    mov x2, #11 // len of prompt
    bl write

    // Read name
    mov x0, #0 // stdin
    ldr x1, =buffer
    mov x2, #100 // maximum bytes to read
    bl read

    // Nuke the newline at the ens of name
    sub x0, x0, #1
    strb wzr, [x1, x0] // '\n' -> 0

    // Read from file //

    // Open file
    bl open
    cmp x0, #0
    blt failed_to_open

    // Read in buffer
    mov x2, #100
    bl read

    // Print read bytes //
    mov x2, x0
    bl write

_end:
    mov x0, #0 // exit code
    mov x8, #93 // syscall: exit
    svc #0

failed_to_open:
    ldr x1, =fail_msg
    mov x2, #20
    bl write
    b _end

write:
    mov x0, #1 // stdout
    mov x8, #64 // syscall: write
    svc #0
    ret

read:
    mov x8, #63 // syscall: read
    svc #0
    ret

open:
    mov x0, #-100 // fd -> AT_FDCWD
    mov x2, 0 // Read flag
    mov x8, #56 // syscall: openat
    svc #0
    ret
