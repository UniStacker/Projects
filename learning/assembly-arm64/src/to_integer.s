.text
.global to_integer

to_integer:
    stp x29, x30, [sp, #-16]! // prologue

    // x0 = bytes of int string
    // x1 = int string buffer
    mov x2, #0 // loop counter
    mov x3, #0 // result
    mov x6, #0 // negative flag
    bl _convert_loop
    mov x0, x3

    // if negative flag, convert
    cbnz x6, _negate

    b _return

    _negate:
        neg x0, x0
        b _return

_convert_loop:
    cmp x2, x0
    bge break
    ldrb w4, [x1, x2]

    // handle negative nums
    cmp w4, #'-'
    beq set_neg_flag

    // if not in 0-9, break
    cmp w4, #'0'
    blt break
    cmp w4, #'9'
    bgt break

    // convert from ascii
    sub w4, w4, #'0'
    mov w5, #10 // temp = 10
    mul x3, x3, x5 // result = result * 10
    add x3, x3, x4   // reault = result + digit

    continue:
        add x2, x2, #1
        b _convert_loop

    break:
        ret

    set_neg_flag:
        mov x6, #1
        b continue

_return:
    ldp x29, x30, [sp], #16 // epilogue
    ret
