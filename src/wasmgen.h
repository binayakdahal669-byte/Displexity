#pragma once
#include "ast.h"
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <map>

/**
 * WasmGenerator - WebAssembly Text Format (WAT) code generation
 * Generates .wat files that can be compiled to .wasm using wat2wasm
 * Targets WASI for I/O operations
 */
namespace disp {

class WasmGenerator {
public:
    WasmGenerator() : m_labelCounter(0), m_localCounter(0) {}
    
    std::string generate(const Program& program) {
        m_code.str("");
        m_labelCounter = 0;
        m_localCounter = 0;
        m_locals.clear();
        m_strings.clear();
        m_stringOffset = 0;
        
        // Scan for strings first
        scanStrings(program);
        
        emitModule(program);
        return m_code.str();
    }
    
private:
    std::ostringstream m_code;
    int m_labelCounter;
    int m_localCounter;
    std::map<std::string, int> m_locals;  // variable name -> local index
    std::vector<std::pair<std::string, int>> m_strings;  // string value -> memory offset
    int m_stringOffset;
    
    void emit(const std::string& s, int indent = 0) {
        for (int i = 0; i < indent; i++) m_code << "  ";
        m_code << s << "\n";
    }
    
    std::string makeLabel() {
        return "$L" + std::to_string(m_labelCounter++);
    }
    
    int addLocal(const std::string& name) {
        if (m_locals.find(name) == m_locals.end()) {
            m_locals[name] = m_localCounter++;
        }
        return m_locals[name];
    }
    
    int addString(const std::string& str) {
        for (const auto& p : m_strings) {
            if (p.first == str) return p.second;
        }
        int offset = m_stringOffset;
        m_strings.push_back({str, offset});
        m_stringOffset += str.length() + 1;  // +1 for null terminator
        return offset;
    }
    
    void scanStrings(const Program& program) {
        for (const auto& stmt : program.statements) {
            scanStmtStrings(stmt);
        }
    }
    
    void scanStmtStrings(const StmtPtr& stmt) {
        if (auto print = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(print->expr)) {
                addString(strLit->value);
            }
        } else if (auto input = std::dynamic_pointer_cast<InputStmt>(stmt)) {
            if (!input->prompt.empty()) {
                addString(input->prompt);
            }
        } else if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
            for (const auto& s : func->body) scanStmtStrings(s);
        } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            for (const auto& s : ifStmt->thenBlock) scanStmtStrings(s);
            for (const auto& s : ifStmt->elseBlock) scanStmtStrings(s);
        } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            for (const auto& s : whileStmt->body) scanStmtStrings(s);
        } else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            for (const auto& s : forStmt->body) scanStmtStrings(s);
        }
    }
    
    void emitModule(const Program& program) {
        emit("(module");
        
        // Import WASI functions for I/O
        emit("  ;; WASI imports for I/O", 1);
        emit("  (import \"wasi_snapshot_preview1\" \"fd_write\"", 1);
        emit("    (func $fd_write (param i32 i32 i32 i32) (result i32)))", 1);
        emit("  (import \"wasi_snapshot_preview1\" \"fd_read\"", 1);
        emit("    (func $fd_read (param i32 i32 i32 i32) (result i32)))", 1);
        emit("  (import \"wasi_snapshot_preview1\" \"proc_exit\"", 1);
        emit("    (func $proc_exit (param i32)))", 1);
        emit("");
        
        // Memory
        emit("  ;; Memory (1 page = 64KB)", 1);
        emit("  (memory (export \"memory\") 1)", 1);
        emit("");
        
        // Data section for strings
        if (!m_strings.empty()) {
            emit("  ;; String data", 1);
            for (const auto& p : m_strings) {
                std::string escaped = escapeWasmString(p.first);
                emit("  (data (i32.const " + std::to_string(p.second) + ") \"" + escaped + "\\00\")", 1);
            }
            emit("");
        }
        
        // Helper function: print string
        emitPrintHelper();
        
        // Helper function: print integer
        emitPrintIntHelper();
        
        // User-defined functions
        for (const auto& stmt : program.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                emitFunction(func);
            }
        }
        
        // Main function (_start for WASI)
        emitMain(program);
        
        emit(")");
    }
    
    void emitPrintHelper() {
        emit("  ;; Helper: print string at offset with length", 1);
        emit("  (func $print_str (param $offset i32) (param $len i32)", 1);
        emit("    ;; Set up iovec at memory location 1024", 2);
        emit("    (i32.store (i32.const 1024) (local.get $offset))", 2);
        emit("    (i32.store (i32.const 1028) (local.get $len))", 2);
        emit("    ;; fd_write(stdout=1, iovec_ptr=1024, iovec_count=1, nwritten_ptr=1032)", 2);
        emit("    (drop (call $fd_write (i32.const 1) (i32.const 1024) (i32.const 1) (i32.const 1032)))", 2);
        emit("  )", 1);
        emit("");
    }
    
    void emitPrintIntHelper() {
        emit("  ;; Helper: print integer", 1);
        emit("  (func $print_int (param $n i32)", 1);
        emit("    (local $digit i32)", 2);
        emit("    (local $ptr i32)", 2);
        emit("    (local $neg i32)", 2);
        emit("    (local.set $ptr (i32.const 1100))", 2);
        emit("    ;; Handle negative", 2);
        emit("    (if (i32.lt_s (local.get $n) (i32.const 0))", 2);
        emit("      (then", 2);
        emit("        (local.set $neg (i32.const 1))", 3);
        emit("        (local.set $n (i32.sub (i32.const 0) (local.get $n)))", 3);
        emit("      )", 2);
        emit("    )", 2);
        emit("    ;; Convert digits (reverse order)", 2);
        emit("    (block $done", 2);
        emit("      (loop $loop", 2);
        emit("        (local.set $digit (i32.rem_u (local.get $n) (i32.const 10)))", 3);
        emit("        (local.set $n (i32.div_u (local.get $n) (i32.const 10)))", 3);
        emit("        (i32.store8 (local.get $ptr) (i32.add (local.get $digit) (i32.const 48)))", 3);
        emit("        (local.set $ptr (i32.add (local.get $ptr) (i32.const 1)))", 3);
        emit("        (br_if $loop (i32.gt_u (local.get $n) (i32.const 0)))", 3);
        emit("      )", 2);
        emit("    )", 2);
        emit("    ;; Add minus sign if negative", 2);
        emit("    (if (local.get $neg)", 2);
        emit("      (then", 2);
        emit("        (i32.store8 (local.get $ptr) (i32.const 45))", 3);
        emit("        (local.set $ptr (i32.add (local.get $ptr) (i32.const 1)))", 3);
        emit("      )", 2);
        emit("    )", 2);
        emit("    ;; Reverse and print", 2);
        emit("    (call $print_reversed (i32.const 1100) (local.get $ptr))", 2);
        emit("  )", 1);
        emit("");
        
        // Helper to print reversed buffer
        emit("  (func $print_reversed (param $start i32) (param $end i32)", 1);
        emit("    (local $len i32)", 2);
        emit("    (local $i i32)", 2);
        emit("    (local $j i32)", 2);
        emit("    (local $tmp i32)", 2);
        emit("    (local.set $len (i32.sub (local.get $end) (local.get $start)))", 2);
        emit("    (local.set $i (local.get $start))", 2);
        emit("    (local.set $j (i32.sub (local.get $end) (i32.const 1)))", 2);
        emit("    ;; Reverse in place", 2);
        emit("    (block $done", 2);
        emit("      (loop $loop", 2);
        emit("        (br_if $done (i32.ge_u (local.get $i) (local.get $j)))", 3);
        emit("        (local.set $tmp (i32.load8_u (local.get $i)))", 3);
        emit("        (i32.store8 (local.get $i) (i32.load8_u (local.get $j)))", 3);
        emit("        (i32.store8 (local.get $j) (local.get $tmp))", 3);
        emit("        (local.set $i (i32.add (local.get $i) (i32.const 1)))", 3);
        emit("        (local.set $j (i32.sub (local.get $j) (i32.const 1)))", 3);
        emit("        (br $loop)", 3);
        emit("      )", 2);
        emit("    )", 2);
        emit("    (call $print_str (local.get $start) (local.get $len))", 2);
        emit("  )", 1);
        emit("");
    }
    
    void emitFunction(std::shared_ptr<FunctionDecl> func) {
        m_locals.clear();
        m_localCounter = 0;
        
        std::string funcName = "$" + func->name;
        std::string params = "";
        for (const auto& p : func->params) {
            addLocal(p.first);
            params += " (param $" + p.first + " i32)";
        }
        
        emit("  (func " + funcName + params + " (result i32)", 1);
        
        // Declare locals
        emit("    (local $__tmp i32)", 2);
        
        for (const auto& stmt : func->body) {
            emitStatement(stmt, 2);
        }
        
        emit("    (i32.const 0)", 2);
        emit("  )", 1);
        emit("");
    }
    
    void emitMain(const Program& program) {
        m_locals.clear();
        m_localCounter = 0;
        
        // Check if user defined main
        bool hasUserMain = false;
        for (const auto& stmt : program.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                if (func->name == "main") {
                    hasUserMain = true;
                    break;
                }
            }
        }
        
        emit("  ;; Entry point", 1);
        emit("  (func $_start (export \"_start\")", 1);
        
        // Declare locals for variables
        emit("    (local $__tmp i32)", 2);
        
        if (hasUserMain) {
            emit("    (drop (call $main))", 2);
        } else {
            // Emit top-level statements
            for (const auto& stmt : program.statements) {
                if (!std::dynamic_pointer_cast<FunctionDecl>(stmt) &&
                    !std::dynamic_pointer_cast<ClassDecl>(stmt)) {
                    emitStatement(stmt, 2);
                }
            }
        }
        
        emit("    (call $proc_exit (i32.const 0))", 2);
        emit("  )", 1);
    }
    
    void emitStatement(const StmtPtr& stmt, int indent) {
        if (auto print = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(print->expr)) {
                int offset = 0;
                for (const auto& p : m_strings) {
                    if (p.first == strLit->value) { offset = p.second; break; }
                }
                int len = strLit->value.length();
                emit("(call $print_str (i32.const " + std::to_string(offset) + ") (i32.const " + std::to_string(len) + "))", indent);
                // Print newline
                emit("(i32.store8 (i32.const 2000) (i32.const 10))", indent);
                emit("(call $print_str (i32.const 2000) (i32.const 1))", indent);
            } else if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(print->expr)) {
                emit("(call $print_int (i32.const " + std::to_string(intLit->value) + "))", indent);
                emit("(i32.store8 (i32.const 2000) (i32.const 10))", indent);
                emit("(call $print_str (i32.const 2000) (i32.const 1))", indent);
            } else if (auto var = std::dynamic_pointer_cast<Variable>(print->expr)) {
                int idx = addLocal(var->name);
                emit("(call $print_int (local.get $" + var->name + "))", indent);
                emit("(i32.store8 (i32.const 2000) (i32.const 10))", indent);
                emit("(call $print_str (i32.const 2000) (i32.const 1))", indent);
            }
        } else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            int idx = addLocal(assign->name);
            emitExpression(assign->value, indent);
            emit("(local.set $" + assign->name + ")", indent);
        } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            emitExpression(ifStmt->condition, indent);
            emit("(if", indent);
            emit("(then", indent + 1);
            for (const auto& s : ifStmt->thenBlock) {
                emitStatement(s, indent + 2);
            }
            emit(")", indent + 1);
            if (!ifStmt->elseBlock.empty()) {
                emit("(else", indent + 1);
                for (const auto& s : ifStmt->elseBlock) {
                    emitStatement(s, indent + 2);
                }
                emit(")", indent + 1);
            }
            emit(")", indent);
        } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            std::string loopLabel = makeLabel();
            std::string endLabel = makeLabel();
            emit("(block " + endLabel, indent);
            emit("(loop " + loopLabel, indent + 1);
            emitExpression(whileStmt->condition, indent + 2);
            emit("(i32.eqz)", indent + 2);
            emit("(br_if " + endLabel + ")", indent + 2);
            for (const auto& s : whileStmt->body) {
                emitStatement(s, indent + 2);
            }
            emit("(br " + loopLabel + ")", indent + 2);
            emit(")", indent + 1);
            emit(")", indent);
        } else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            int idx = addLocal(forStmt->var);
            std::string loopLabel = makeLabel();
            std::string endLabel = makeLabel();
            // Initialize
            emitExpression(forStmt->start, indent);
            emit("(local.set $" + forStmt->var + ")", indent);
            emit("(block " + endLabel, indent);
            emit("(loop " + loopLabel, indent + 1);
            // Check condition
            emit("(local.get $" + forStmt->var + ")", indent + 2);
            emitExpression(forStmt->end, indent + 2);
            emit("(i32.ge_s)", indent + 2);
            emit("(br_if " + endLabel + ")", indent + 2);
            // Body
            for (const auto& s : forStmt->body) {
                emitStatement(s, indent + 2);
            }
            // Increment
            emit("(local.get $" + forStmt->var + ")", indent + 2);
            emit("(i32.const 1)", indent + 2);
            emit("(i32.add)", indent + 2);
            emit("(local.set $" + forStmt->var + ")", indent + 2);
            emit("(br " + loopLabel + ")", indent + 2);
            emit(")", indent + 1);
            emit(")", indent);
        } else if (auto retStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
            if (retStmt->value) {
                emitExpression(retStmt->value, indent);
            } else {
                emit("(i32.const 0)", indent);
            }
            emit("(return)", indent);
        }
    }
    
    void emitExpression(const ExprPtr& expr, int indent) {
        if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(expr)) {
            emit("(i32.const " + std::to_string(intLit->value) + ")", indent);
        } else if (auto boolLit = std::dynamic_pointer_cast<BoolLiteral>(expr)) {
            emit("(i32.const " + std::string(boolLit->value ? "1" : "0") + ")", indent);
        } else if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            addLocal(var->name);
            emit("(local.get $" + var->name + ")", indent);
        } else if (auto binOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
            emitExpression(binOp->left, indent);
            emitExpression(binOp->right, indent);
            if (binOp->op == "+") emit("(i32.add)", indent);
            else if (binOp->op == "-") emit("(i32.sub)", indent);
            else if (binOp->op == "*") emit("(i32.mul)", indent);
            else if (binOp->op == "/") emit("(i32.div_s)", indent);
            else if (binOp->op == "%") emit("(i32.rem_s)", indent);
            else if (binOp->op == "==") emit("(i32.eq)", indent);
            else if (binOp->op == "!=") emit("(i32.ne)", indent);
            else if (binOp->op == "<") emit("(i32.lt_s)", indent);
            else if (binOp->op == "<=") emit("(i32.le_s)", indent);
            else if (binOp->op == ">") emit("(i32.gt_s)", indent);
            else if (binOp->op == ">=") emit("(i32.ge_s)", indent);
            else if (binOp->op == "&&") emit("(i32.and)", indent);
            else if (binOp->op == "||") emit("(i32.or)", indent);
        } else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
            emitExpression(unaryOp->expr, indent);
            if (unaryOp->op == "-") {
                emit("(i32.const 0)", indent);
                emit("(i32.sub)", indent);
            } else if (unaryOp->op == "!") {
                emit("(i32.eqz)", indent);
            }
        } else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
            for (const auto& arg : funcCall->args) {
                emitExpression(arg, indent);
            }
            emit("(call $" + funcCall->name + ")", indent);
        }
    }
    
    std::string escapeWasmString(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else if (c == '\t') result += "\\t";
            else if (c < 32 || c > 126) {
                // Hex escape for non-printable
                char buf[8];
                snprintf(buf, sizeof(buf), "\\%02x", (unsigned char)c);
                result += buf;
            } else {
                result += c;
            }
        }
        return result;
    }
};

}  // namespace disp
