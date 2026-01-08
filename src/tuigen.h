#ifndef DISPLEXITY_TUIGEN_H
#define DISPLEXITY_TUIGEN_H

#include "ast.h"
#include <sstream>
#include <vector>
#include <map>
#include <cstdint>

namespace disp {

// TUI Bytecode Instructions
enum class TuiOpcode : uint8_t {
    // Stack operations
    NOP = 0x00,
    PUSH_INT = 0x01,
    PUSH_FLOAT = 0x02,
    PUSH_STR = 0x03,
    PUSH_BOOL = 0x04,
    POP = 0x05,
    DUP = 0x06,
    SWAP = 0x07,
    
    // Arithmetic
    ADD = 0x10,
    SUB = 0x11,
    MUL = 0x12,
    DIV = 0x13,
    MOD = 0x14,
    NEG = 0x15,
    
    // Comparison
    EQ = 0x20,
    NE = 0x21,
    LT = 0x22,
    GT = 0x23,
    LE = 0x24,
    GE = 0x25,
    
    // Logical
    AND = 0x30,
    OR = 0x31,
    NOT = 0x32,
    
    // Control flow
    JMP = 0x40,
    JZ = 0x41,
    JNZ = 0x42,
    CALL = 0x43,
    RET = 0x44,
    
    // Variables
    LOAD = 0x50,
    STORE = 0x51,
    LOAD_GLOBAL = 0x52,
    STORE_GLOBAL = 0x53,
    
    // I/O
    PRINT = 0x60,
    PRINTLN = 0x61,
    INPUT = 0x62,
    INPUT_INT = 0x63,
    INPUT_FLOAT = 0x64,
    
    // Terminal UI
    CLEAR = 0x70,
    GOTOXY = 0x71,
    SET_COLOR = 0x72,
    GET_KEY = 0x73,
    SLEEP = 0x74,
    
    // System
    HALT = 0xFF
};

// TUI File Format Constants
const char TUI_MAGIC[8] = {'D', 'I', 'S', 'P', 'L', 'T', 'U', 'I'};
const uint32_t TUI_VERSION = 1;

struct TuiHeader {
    char magic[8];
    uint32_t version;
    uint32_t flags;
    uint64_t checksum;
    uint64_t timestamp;
    uint64_t bytecodeSize;
    uint64_t dataSize;
    uint32_t entryPoint;
    uint32_t numFunctions;
    uint32_t numStrings;
};

struct TuiFunctionEntry {
    uint32_t nameOffset;
    uint32_t bytecodeOffset;
    uint32_t numParams;
    uint32_t numLocals;
};

class TuiGenerator {
private:
    std::vector<uint8_t> bytecode;
    std::vector<std::string> stringPool;
    std::map<std::string, uint32_t> stringIndex;
    std::map<std::string, uint32_t> functionOffsets;
    std::map<std::string, uint32_t> variableSlots;
    std::vector<TuiFunctionEntry> functions;
    uint32_t currentSlot = 0;
    uint32_t entryPoint = 0;
    
    uint32_t addString(const std::string& str) {
        auto it = stringIndex.find(str);
        if (it != stringIndex.end()) return it->second;
        uint32_t idx = stringPool.size();
        stringPool.push_back(str);
        stringIndex[str] = idx;
        return idx;
    }
    
    void emit(TuiOpcode op) {
        bytecode.push_back(static_cast<uint8_t>(op));
    }
    
    void emitByte(uint8_t b) {
        bytecode.push_back(b);
    }
    
    void emitInt32(int32_t val) {
        bytecode.push_back(val & 0xFF);
        bytecode.push_back((val >> 8) & 0xFF);
        bytecode.push_back((val >> 16) & 0xFF);
        bytecode.push_back((val >> 24) & 0xFF);
    }
    
    void emitUint32(uint32_t val) {
        bytecode.push_back(val & 0xFF);
        bytecode.push_back((val >> 8) & 0xFF);
        bytecode.push_back((val >> 16) & 0xFF);
        bytecode.push_back((val >> 24) & 0xFF);
    }
    
    void emitFloat(float val) {
        union { float f; uint32_t i; } u;
        u.f = val;
        emitUint32(u.i);
    }
    
    uint32_t currentOffset() {
        return bytecode.size();
    }
    
    void patchJump(uint32_t offset, uint32_t target) {
        bytecode[offset] = target & 0xFF;
        bytecode[offset + 1] = (target >> 8) & 0xFF;
        bytecode[offset + 2] = (target >> 16) & 0xFF;
        bytecode[offset + 3] = (target >> 24) & 0xFF;
    }
    
    uint32_t getVarSlot(const std::string& name) {
        auto it = variableSlots.find(name);
        if (it != variableSlots.end()) return it->second;
        uint32_t slot = currentSlot++;
        variableSlots[name] = slot;
        return slot;
    }

public:
    std::vector<uint8_t> generate(const Program& prog) {
        bytecode.clear();
        stringPool.clear();
        stringIndex.clear();
        functionOffsets.clear();
        variableSlots.clear();
        functions.clear();
        currentSlot = 0;
        
        // First pass: collect function declarations
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                functionOffsets[func->name] = 0; // Will be filled in second pass
            }
        }
        
        // Generate code for functions
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                generateFunction(func);
            }
        }
        
        // Generate main code (entry point)
        entryPoint = currentOffset();
        for (const auto& stmt : prog.statements) {
            if (!std::dynamic_pointer_cast<FunctionDecl>(stmt) &&
                !std::dynamic_pointer_cast<ClassDecl>(stmt)) {
                generateStatement(stmt);
            }
        }
        emit(TuiOpcode::HALT);
        
        // Build final binary
        return buildBinary();
    }
    
private:
    void generateFunction(const std::shared_ptr<FunctionDecl>& func) {
        functionOffsets[func->name] = currentOffset();
        
        // Set up parameter slots
        variableSlots.clear();
        currentSlot = 0;
        for (const auto& param : func->params) {
            variableSlots[param.first] = currentSlot++;
        }
        
        TuiFunctionEntry entry;
        entry.nameOffset = addString(func->name);
        entry.bytecodeOffset = currentOffset();
        entry.numParams = func->params.size();
        
        // Generate function body
        for (const auto& stmt : func->body) {
            generateStatement(stmt);
        }
        
        // Default return
        emit(TuiOpcode::PUSH_INT);
        emitInt32(0);
        emit(TuiOpcode::RET);
        
        entry.numLocals = currentSlot;
        functions.push_back(entry);
    }
    
    void generateStatement(const StmtPtr& stmt) {
        if (auto print = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            generateExpression(print->expr);
            emit(TuiOpcode::PRINTLN);
        }
        else if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(stmt)) {
            uint32_t slot = getVarSlot(varDecl->name);
            if (varDecl->initialValue) {
                generateExpression(varDecl->initialValue);
            } else {
                emit(TuiOpcode::PUSH_INT);
                emitInt32(0);
            }
            emit(TuiOpcode::STORE);
            emitUint32(slot);
        }
        else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            generateExpression(assign->value);
            uint32_t slot = getVarSlot(assign->name);
            emit(TuiOpcode::STORE);
            emitUint32(slot);
        }
        else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            generateExpression(ifStmt->condition);
            emit(TuiOpcode::JZ);
            uint32_t elseJump = currentOffset();
            emitUint32(0); // Placeholder
            
            for (const auto& s : ifStmt->thenBlock) {
                generateStatement(s);
            }
            
            if (!ifStmt->elseBlock.empty()) {
                emit(TuiOpcode::JMP);
                uint32_t endJump = currentOffset();
                emitUint32(0); // Placeholder
                
                patchJump(elseJump, currentOffset());
                for (const auto& s : ifStmt->elseBlock) {
                    generateStatement(s);
                }
                patchJump(endJump, currentOffset());
            } else {
                patchJump(elseJump, currentOffset());
            }
        }
        else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            uint32_t loopStart = currentOffset();
            generateExpression(whileStmt->condition);
            emit(TuiOpcode::JZ);
            uint32_t exitJump = currentOffset();
            emitUint32(0); // Placeholder
            
            for (const auto& s : whileStmt->body) {
                generateStatement(s);
            }
            
            emit(TuiOpcode::JMP);
            emitUint32(loopStart);
            patchJump(exitJump, currentOffset());
        }
        else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            // Initialize loop variable
            uint32_t slot = getVarSlot(forStmt->var);
            generateExpression(forStmt->start);
            emit(TuiOpcode::STORE);
            emitUint32(slot);
            
            uint32_t loopStart = currentOffset();
            
            // Check condition (var < end)
            emit(TuiOpcode::LOAD);
            emitUint32(slot);
            generateExpression(forStmt->end);
            emit(TuiOpcode::LT);
            emit(TuiOpcode::JZ);
            uint32_t exitJump = currentOffset();
            emitUint32(0); // Placeholder
            
            // Body
            for (const auto& s : forStmt->body) {
                generateStatement(s);
            }
            
            // Increment
            emit(TuiOpcode::LOAD);
            emitUint32(slot);
            emit(TuiOpcode::PUSH_INT);
            emitInt32(1);
            emit(TuiOpcode::ADD);
            emit(TuiOpcode::STORE);
            emitUint32(slot);
            
            emit(TuiOpcode::JMP);
            emitUint32(loopStart);
            patchJump(exitJump, currentOffset());
        }
        else if (auto ret = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
            if (ret->value) {
                generateExpression(ret->value);
            } else {
                emit(TuiOpcode::PUSH_INT);
                emitInt32(0);
            }
            emit(TuiOpcode::RET);
        }
        else if (auto input = std::dynamic_pointer_cast<InputStmt>(stmt)) {
            if (!input->prompt.empty()) {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString(input->prompt));
                emit(TuiOpcode::PRINT);
            }
            emit(TuiOpcode::INPUT);
            uint32_t slot = getVarSlot(input->resultVar);
            emit(TuiOpcode::STORE);
            emitUint32(slot);
        }
        else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(stmt)) {
            // Function call as statement - generate and discard result
            generateExpression(funcCall);
            emit(TuiOpcode::POP);
        }
    }
    
    void generateExpression(const ExprPtr& expr) {
        if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(expr)) {
            emit(TuiOpcode::PUSH_INT);
            emitInt32(intLit->value);
        }
        else if (auto floatLit = std::dynamic_pointer_cast<FloatLiteral>(expr)) {
            emit(TuiOpcode::PUSH_FLOAT);
            emitFloat(floatLit->value);
        }
        else if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(expr)) {
            emit(TuiOpcode::PUSH_STR);
            emitUint32(addString(strLit->value));
        }
        else if (auto boolLit = std::dynamic_pointer_cast<BoolLiteral>(expr)) {
            emit(TuiOpcode::PUSH_BOOL);
            emitByte(boolLit->value ? 1 : 0);
        }
        else if (auto infLit = std::dynamic_pointer_cast<InfinityLiteral>(expr)) {
            // Push a very large integer to represent infinity
            emit(TuiOpcode::PUSH_INT);
            emitInt32(2147483647); // Max 32-bit signed integer
        }
        else if (auto varRef = std::dynamic_pointer_cast<Variable>(expr)) {
            auto it = variableSlots.find(varRef->name);
            if (it != variableSlots.end()) {
                emit(TuiOpcode::LOAD);
                emitUint32(it->second);
            } else {
                // Global or undefined - push 0
                emit(TuiOpcode::PUSH_INT);
                emitInt32(0);
            }
        }
        else if (auto binOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
            generateExpression(binOp->left);
            generateExpression(binOp->right);
            
            if (binOp->op == "+") emit(TuiOpcode::ADD);
            else if (binOp->op == "-") emit(TuiOpcode::SUB);
            else if (binOp->op == "*") emit(TuiOpcode::MUL);
            else if (binOp->op == "/") emit(TuiOpcode::DIV);
            else if (binOp->op == "%") emit(TuiOpcode::MOD);
            else if (binOp->op == "==" || binOp->op == "is") emit(TuiOpcode::EQ);
            else if (binOp->op == "!=" || binOp->op == "isnt") emit(TuiOpcode::NE);
            else if (binOp->op == "<") emit(TuiOpcode::LT);
            else if (binOp->op == ">") emit(TuiOpcode::GT);
            else if (binOp->op == "<=") emit(TuiOpcode::LE);
            else if (binOp->op == ">=") emit(TuiOpcode::GE);
            else if (binOp->op == "and" || binOp->op == "&&") emit(TuiOpcode::AND);
            else if (binOp->op == "or" || binOp->op == "||") emit(TuiOpcode::OR);
        }
        else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
            generateExpression(unaryOp->expr);
            if (unaryOp->op == "-") emit(TuiOpcode::NEG);
            else if (unaryOp->op == "!" || unaryOp->op == "not") emit(TuiOpcode::NOT);
        }
        else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
            // Push arguments in reverse order
            for (auto it = funcCall->args.rbegin(); it != funcCall->args.rend(); ++it) {
                generateExpression(*it);
            }
            
            // Built-in functions
            if (funcCall->name == "clear") {
                emit(TuiOpcode::CLEAR);
            } else if (funcCall->name == "gotoxy") {
                emit(TuiOpcode::GOTOXY);
            } else if (funcCall->name == "set_color") {
                emit(TuiOpcode::SET_COLOR);
            } else if (funcCall->name == "get_key") {
                emit(TuiOpcode::GET_KEY);
            } else if (funcCall->name == "sleep") {
                emit(TuiOpcode::SLEEP);
            } else if (funcCall->name == "enhanced_tui_init") {
                // Enhanced TUI initialization - store parameters
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("enhanced_init"));
            } else if (funcCall->name == "enhanced_clear") {
                emit(TuiOpcode::CLEAR);
            } else if (funcCall->name == "enhanced_present") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("enhanced_present"));
            } else if (funcCall->name == "maintain_frame_rate") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("maintain_fps"));
            } else if (funcCall->name == "load_texture") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("load_tex"));
            } else if (funcCall->name == "load_sound_dish" || funcCall->name == "load_sound_disll") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("load_sound"));
            } else if (funcCall->name == "play_sound") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("play_sound"));
            } else if (funcCall->name == "generate_voxel_chunk") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("gen_chunk"));
            } else if (funcCall->name == "enhanced_render_textured" || funcCall->name == "enhanced_render_raytraced") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("render_3d"));
            } else if (funcCall->name == "enhanced_draw_text") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("draw_text"));
            } else if (funcCall->name == "enhanced_draw_box") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("draw_box"));
            } else if (funcCall->name == "set_camera_pos" || funcCall->name == "set_camera_rotation") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("camera"));
            } else if (funcCall->name == "mat4_projection" || funcCall->name == "mat4_identity") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("matrix"));
            } else if (funcCall->name == "get_time_ms") {
                emit(TuiOpcode::PUSH_INT);
                emitInt32((int)(time(nullptr) * 1000));
            } else if (funcCall->name == "toggle_raytracing") {
                emit(TuiOpcode::PUSH_STR);
                emitUint32(addString("toggle_rt"));
            } else {
                // User function call
                auto it = functionOffsets.find(funcCall->name);
                if (it != functionOffsets.end() && it->second != 0) {
                    emit(TuiOpcode::CALL);
                    emitUint32(it->second);
                    emitByte(funcCall->args.size());
                } else {
                    // Unknown function - just push a dummy value and pop args
                    // This prevents infinite loops from calling offset 0
                    for (size_t i = 0; i < funcCall->args.size(); i++) {
                        emit(TuiOpcode::POP); // Pop the arguments we pushed
                    }
                    emit(TuiOpcode::PUSH_STR);
                    emitUint32(addString("[undefined:" + funcCall->name + "]"));
                }
            }
        }
    }
    
    std::vector<uint8_t> buildBinary() {
        std::vector<uint8_t> binary;
        
        // Build string data section
        std::vector<uint8_t> stringData;
        std::vector<uint32_t> stringOffsets;
        for (const auto& str : stringPool) {
            stringOffsets.push_back(stringData.size());
            for (char c : str) {
                stringData.push_back(c);
            }
            stringData.push_back(0); // Null terminator
        }
        
        // Calculate header
        TuiHeader header;
        memcpy(header.magic, TUI_MAGIC, 8);
        header.version = TUI_VERSION;
        header.flags = 0;
        header.timestamp = time(nullptr);
        header.bytecodeSize = bytecode.size();
        header.dataSize = stringData.size();
        header.entryPoint = entryPoint;
        header.numFunctions = functions.size();
        header.numStrings = stringPool.size();
        
        // Calculate checksum
        uint64_t checksum = 0;
        for (uint8_t b : bytecode) {
            checksum = checksum * 31 + b;
        }
        for (uint8_t b : stringData) {
            checksum = checksum * 31 + b;
        }
        header.checksum = checksum;
        
        // Write header
        binary.insert(binary.end(), (uint8_t*)&header, (uint8_t*)&header + sizeof(header));
        
        // Write function table
        for (const auto& func : functions) {
            binary.insert(binary.end(), (uint8_t*)&func, (uint8_t*)&func + sizeof(func));
        }
        
        // Write string offsets
        for (uint32_t offset : stringOffsets) {
            binary.push_back(offset & 0xFF);
            binary.push_back((offset >> 8) & 0xFF);
            binary.push_back((offset >> 16) & 0xFF);
            binary.push_back((offset >> 24) & 0xFF);
        }
        
        // Write bytecode
        binary.insert(binary.end(), bytecode.begin(), bytecode.end());
        
        // Write string data
        binary.insert(binary.end(), stringData.begin(), stringData.end());
        
        return binary;
    }
};

} // namespace disp

#endif // DISPLEXITY_TUIGEN_H
