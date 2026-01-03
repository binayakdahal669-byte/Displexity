#pragma once
#include "ast.h"
#include <string>
#include <vector>
#include <sstream>
#include <memory>

/**
 * ArmGenerator - ARM64 (aarch64) assembly code generation
 * Targets ARM 64-bit EABI calling convention
 * Output: x0-x7 for arguments, x0 for return value
 */
namespace disp {

class ArmGenerator {
public:
    ArmGenerator() : m_indent(0), m_labelCounter(0), m_stackOffset(0) {}
    
    std::string generate(const Program& program) {
        m_code.clear();
        m_indent = 0;
        m_labelCounter = 0;
        m_stackOffset = 0;
        
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
    int m_stackOffset;
    
    void emit(const std::string& s) {
        for (int i = 0; i < m_indent; i++) m_code << "    ";
        m_code << s << "\n";
    }
    
    std::string makeLabel() {
        return ".L" + std::to_string(m_labelCounter++);
    }
    
    void emitHeader() {
        emit(".section .text");
        emit(".global main");
        emit(".align 2");
    }
    
    void emitDataSection() {
        emit("");
        emit(".section .data");
        emit("str_displexity: .string \"Displexity\"");
    }
    
    void emitMain() {
        emit("");
        emit(".section .text");
        emit("main:");
        m_indent++;
        
        // ARM64 function prologue
        emit("stp x29, x30, [sp, #-16]!");  // Save frame pointer and link register
        emit("mov x29, sp");                 // Set frame pointer
        
        // Simple implementation: print message and return
        emit("adrp x0, str_displexity");     // Load address of string (high part)
        emit("add x0, x0, :lo12:str_displexity");  // Add low 12 bits
        emit("bl printf");                    // Call printf
        
        // ARM64 function epilogue
        emit("mov x0, #0");                  // Return 0
        emit("ldp x29, x30, [sp], #16");    // Restore frame pointer and link register
        emit("ret");                         // Return to caller
        
        m_indent--;
    }
    
    void emitFooter() {
        emit("");
        emit("; === ARM64 ABI Notes ===");
        emit("; Argument passing: x0-x7");
        emit("; Return value: x0");
        emit("; Callee-saved: x19-x28, sp, x29, x30");
        emit("; Stack must be 16-byte aligned");
    }
};

}  // namespace disp
