section .data
    str_fmt: db "%s", 0
    int_fmt: db "%d", 0
    newline: db 10, 0

section .text
extern printf
extern malloc
extern free
global main

main:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    ; print statement
    lea rdi, [rel str_fmt]
    mov rsi, "Hello, Displexity!"
    xor eax, eax
    call printf
    xor eax, eax
    leave
    ret
