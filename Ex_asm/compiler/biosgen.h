#pragma once
#include "ast.h"
#include <string>
#include <vector>
#include <sstream>
#include <memory>

/**
 * BiosGenerator - x86-16 assembly targeting bare-metal BIOS boot
 * Generates bootloader code that prints output and handles basic I/O
 * Output: x86-16 real-mode assembly suitable for .bin boot sector
 */
namespace disp {

class BiosGenerator {
public:
    BiosGenerator() : m_indent(0), m_labelCounter(0) {}
    
    std::string generate(const Program& program) {
        m_code.clear();
        m_indent = 0;
        m_labelCounter = 0;
        
        emitHeader();
        
        // For now, just emit a simple boot message
        // Full AST traversal to be implemented
        emitSimpleMain();
        
        emitFooter();
        return m_code.str();
    }
    
private:
    std::ostringstream m_code;
    int m_indent;
    int m_labelCounter;
    
    void emit(const std::string& s) {
        for (int i = 0; i < m_indent; i++) m_code << "    ";
        m_code << s << "\n";
    }
    
    std::string makeLabel() {
        return "L" + std::to_string(m_labelCounter++);
    }
    
    void emitHeader() {
        emit("BITS 16                    ; x86-16 real mode");
        emit("ORG 0x7C00                 ; Boot sector origin");
        emit("");
        emit("; ===== BIOS BOOTLOADER =====");
        emit("jmp start                  ; Jump over data");
        emit("");
        emit("start:");
        emit("    mov ax, 0x0000        ; Clear registers");
        emit("    mov ds, ax");
        emit("    mov es, ax");
        emit("    mov ss, ax");
        emit("    mov sp, 0x7C00");
        emit("");
        emit("    call main              ; Call Displexity main");
        emit("    jmp $                  ; Halt");
        emit("");
    }
    
    void emitSimpleMain() {
        // Simple main that prints "Displexity"
        emit("main:");
        m_indent++;
        emit("mov si, msg");
        emit("call print_string");
        emit("ret");
        m_indent--;
        emit("");
        emit("msg: db 'Displexity Bootloader', 0");
    }
    
    void emitFooter() {
        emit("");
        emit("; ===== RUNTIME HELPERS =====");
        emit("");
        emit("print_string:");
        emit("    lodsb");
        emit("    or al, al");
        emit("    jz .ps_done");
        emit("    mov ah, 0x0E");
        emit("    int 0x10");
        emit("    jmp print_string");
        emit(".ps_done:");
        emit("    ret");
        emit("");
        emit("print_number:");
        emit("    mov bx, 10");
        emit("    xor cx, cx");
        emit(".pn_loop:");
        emit("    xor dx, dx");
        emit("    div bx");
        emit("    push dx");
        emit("    inc cx");
        emit("    or ax, ax");
        emit("    jnz .pn_loop");
        emit(".pn_print:");
        emit("    pop ax");
        emit("    add al, '0'");
        emit("    mov ah, 0x0E");
        emit("    int 0x10");
        emit("    loop .pn_print");
        emit("    ret");
        emit("");
        emit("; ===== PADDING & SIGNATURE =====");
        emit("times (0x1FE - ($ - $$)) db 0");
        emit("dw 0xAA55                 ; Boot signature");
    }
};

}  // namespace disp
