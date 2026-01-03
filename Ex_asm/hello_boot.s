BITS 16                    ; x86-16 real mode
ORG 0x7C00                 ; Boot sector origin

; ===== BIOS BOOTLOADER =====
jmp start                  ; Jump over data

start:
    mov ax, 0x0000        ; Clear registers
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    call main              ; Call Displexity main
    jmp $                  ; Halt

main:
    mov si, msg
    call print_string
    ret

msg: db 'Displexity Bootloader', 0

; ===== RUNTIME HELPERS =====

print_string:
    lodsb
    or al, al
    jz .ps_done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.ps_done:
    ret

print_number:
    mov bx, 10
    xor cx, cx
.pn_loop:
    xor dx, dx
    div bx
    push dx
    inc cx
    or ax, ax
    jnz .pn_loop
.pn_print:
    pop ax
    add al, '0'
    mov ah, 0x0E
    int 0x10
    loop .pn_print
    ret

; ===== PADDING & SIGNATURE =====
times (0x1FE - ($ - $$)) db 0
dw 0xAA55                 ; Boot signature
