.section .text
.global main
.align 2

.section .data
str_displexity: .string "Displexity"

.section .text
main:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    adrp x0, str_displexity
    add x0, x0, :lo12:str_displexity
    bl printf
    mov x0, #0
    ldp x29, x30, [sp], #16
    ret

; === ARM64 ABI Notes ===
; Argument passing: x0-x7
; Return value: x0
; Callee-saved: x19-x28, sp, x29, x30
; Stack must be 16-byte aligned
