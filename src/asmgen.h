#ifndef DISPLEXITY_ASMGEN_H
#define DISPLEXITY_ASMGEN_H

#include "ast.h"
#include <sstream>
#include <map>
#include <set>

namespace disp {

class AsmGenerator {
private:
    std::stringstream asm_code;
    std::map<std::string, int> var_stack_offset;
    int stack_offset = 0;
    int tmp_counter = 0;

    std::string genTmp() {
        return "__tmp_" + std::to_string(tmp_counter++);
    }

    int allocStack(const std::string& var, int size = 8) {
        stack_offset += size;
        var_stack_offset[var] = -stack_offset;
        return -stack_offset;
    }

public:
    std::string generate(const Program& prog) {
        asm_code.str("");
        asm_code << "section .data\n";
        asm_code << "    str_fmt: db \"%s\", 0\n";
        asm_code << "    int_fmt: db \"%d\", 0\n";
        asm_code << "    newline: db 10, 0\n\n";

        asm_code << "section .text\n";
        asm_code << "extern printf\n";
        asm_code << "extern malloc\n";
        asm_code << "extern free\n";
        asm_code << "global main\n\n";

        // Pre-declare functions
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                asm_code << func->name << ":\n";
                emitFunctionBody(func);
            }
        }

        // Main function
        asm_code << "main:\n";
        asm_code << "    push rbp\n";
        asm_code << "    mov rbp, rsp\n";
        asm_code << "    sub rsp, 256\n";  // Allocate stack space

        for (const auto& stmt : prog.statements) {
            if (!std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                emitStatement(stmt);
            }
        }

        asm_code << "    xor eax, eax\n";
        asm_code << "    leave\n";
        asm_code << "    ret\n";

        return asm_code.str();
    }

private:
    void emitFunctionBody(std::shared_ptr<FunctionDecl> func) {
        asm_code << "    push rbp\n";
        asm_code << "    mov rbp, rsp\n";
        asm_code << "    sub rsp, 256\n";

        stack_offset = 0;
        var_stack_offset.clear();

        for (const auto& stmt : func->body) {
            emitStatement(stmt);
        }

        asm_code << "    xor eax, eax\n";
        asm_code << "    leave\n";
        asm_code << "    ret\n\n";
    }

    void emitStatement(const StmtPtr& stmt) {
        if (auto print = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            asm_code << "    ; print statement\n";
            if (auto str = std::dynamic_pointer_cast<StringLiteral>(print->expr)) {
                // For simplicity, emit a simple printf call
                asm_code << "    lea rdi, [rel str_fmt]\n";
                asm_code << "    mov rsi, \"" << str->value << "\"\n";
                asm_code << "    xor eax, eax\n";
                asm_code << "    call printf\n";
            }
        } else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            asm_code << "    ; assign " << assign->name << "\n";
            allocStack(assign->name);
        }
    }
};

} // namespace disp

#endif // DISPLEXITY_ASMGEN_H
