#pragma once
#include "ast.h"
#include <string>
#include <vector>
#include <sstream>
#include <memory>

/**
 * RiscVGenerator - RISC-V RV64I assembly code generation
 * Targets 64-bit RISC-V with integer extension
 * Output: a0-a7 for arguments, a0 for return value
 */
namespace disp {

class RiscVGenerator {
public:
    RiscVGenerator() : m_indent(0), m_labelCounter(0) {}
    
    std::string generate(const Program& program) {
        m_code.clear();
        m_indent = 0;
        m_labelCounter = 0;
        
        emitHeader();
        
        // Emit data section
        emitDataSection();
        
        // Emit main function
        emitMain();
        
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
        return ".L" + std::to_string(m_labelCounter++);
    }
    
    void emitHeader() {
        emit(".section .text");
        emit(".globl main");
        emit(".align 2");
    }
    
    void emitDataSection() {
        emit("");
        emit(".section .data");
        emit("str_displexity:");
        m_indent++;
        emit(".string \"Displexity RISC-V\"");
        m_indent--;
    }
    
    void emitMain() {
        emit("");
        emit(".section .text");
        emit("main:");
        m_indent++;
        
        // RISC-V RV64I function prologue
        emit("addi sp, sp, -16        # Allocate 16 bytes on stack");
        emit("sd ra, 8(sp)            # Save return address");
        emit("sd s0, 0(sp)            # Save frame pointer");
        emit("addi s0, sp, 16         # Set frame pointer");
        
        // Simple implementation: print message and return
        emit("");
        emit("la a0, str_displexity   # Load address of string");
        emit("jal ra, printf          # Call printf");
        
        // RISC-V RV64I function epilogue
        emit("");
        emit("li a0, 0                # Return 0");
        emit("ld s0, 0(sp)            # Restore frame pointer");
        emit("ld ra, 8(sp)            # Restore return address");
        emit("addi sp, sp, 16         # Deallocate stack");
        emit("ret                     # Return to caller");
        
        m_indent--;
    }
    
    void emitFooter() {
        emit("");
        emit("# === RISC-V RV64I ABI Notes ===");
        emit("# Argument passing: a0-a7");
        emit("# Return value: a0");
        emit("# Caller-saved: a0-a7, t0-t6");
        emit("# Callee-saved: s0-s11, sp");
        emit("# Stack must be 16-byte aligned");
        emit("# Return address: ra (x1)");
    }
};

}  // namespace disp
