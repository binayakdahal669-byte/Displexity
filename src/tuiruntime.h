#ifndef DISPLEXITY_TUIRUNTIME_H
#define DISPLEXITY_TUIRUNTIME_H

#include "tuigen.h"
#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

namespace disp {

// Value type for TUI VM stack
struct TuiValue {
    enum Type { INT, FLOAT, STRING, BOOL } type;
    union {
        int32_t intVal;
        float floatVal;
        bool boolVal;
    };
    std::string strVal;
    
    TuiValue() : type(INT), intVal(0) {}
    TuiValue(int32_t v) : type(INT), intVal(v) {}
    TuiValue(float v) : type(FLOAT), floatVal(v) {}
    TuiValue(bool v) : type(BOOL), boolVal(v) {}
    TuiValue(const std::string& v) : type(STRING), intVal(0), strVal(v) {}
    
    int32_t toInt() const {
        switch (type) {
            case INT: return intVal;
            case FLOAT: return (int32_t)floatVal;
            case BOOL: return boolVal ? 1 : 0;
            case STRING: return std::stoi(strVal);
        }
        return 0;
    }
    
    float toFloat() const {
        switch (type) {
            case INT: return (float)intVal;
            case FLOAT: return floatVal;
            case BOOL: return boolVal ? 1.0f : 0.0f;
            case STRING: return std::stof(strVal);
        }
        return 0.0f;
    }
    
    bool toBool() const {
        switch (type) {
            case INT: return intVal != 0;
            case FLOAT: return floatVal != 0.0f;
            case BOOL: return boolVal;
            case STRING: return !strVal.empty();
        }
        return false;
    }
    
    std::string toString() const {
        switch (type) {
            case INT: return std::to_string(intVal);
            case FLOAT: return std::to_string(floatVal);
            case BOOL: return boolVal ? "true" : "false";
            case STRING: return strVal;
        }
        return "";
    }
};

class TuiRuntime {
private:
    std::vector<uint8_t> bytecode;
    std::vector<std::string> strings;
    std::vector<TuiFunctionEntry> functions;
    uint32_t entryPoint;
    
    std::stack<TuiValue> stack;
    std::vector<TuiValue> locals;
    std::stack<uint32_t> callStack;
    std::stack<uint32_t> frameStack;
    
    uint32_t ip; // Instruction pointer
    bool running;
    
    uint8_t readByte() {
        return bytecode[ip++];
    }
    
    int32_t readInt32() {
        int32_t val = bytecode[ip] | (bytecode[ip+1] << 8) | 
                      (bytecode[ip+2] << 16) | (bytecode[ip+3] << 24);
        ip += 4;
        return val;
    }
    
    uint32_t readUint32() {
        uint32_t val = bytecode[ip] | (bytecode[ip+1] << 8) | 
                       (bytecode[ip+2] << 16) | (bytecode[ip+3] << 24);
        ip += 4;
        return val;
    }
    
    float readFloat() {
        union { float f; uint32_t i; } u;
        u.i = readUint32();
        return u.f;
    }
    
    TuiValue pop() {
        if (stack.empty()) return TuiValue(0);
        TuiValue v = stack.top();
        stack.pop();
        return v;
    }
    
    void push(const TuiValue& v) {
        stack.push(v);
    }
    
    // Terminal functions
    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[H";
#endif
    }
    
    void gotoXY(int x, int y) {
#ifdef _WIN32
        COORD coord = {(SHORT)x, (SHORT)y};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
        std::cout << "\033[" << (y+1) << ";" << (x+1) << "H";
#endif
    }
    
    void setColor(int color) {
#ifdef _WIN32
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
        // ANSI color codes
        int fg = color & 0x0F;
        int bg = (color >> 4) & 0x0F;
        std::cout << "\033[" << (30 + (fg & 7)) << ";" << (40 + (bg & 7)) << "m";
#endif
    }
    
    int getKey() {
#ifdef _WIN32
        if (_kbhit()) {
            return _getch();
        }
        return 0;
#else
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        newt.c_cc[VMIN] = 0;
        newt.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch == EOF ? 0 : ch;
#endif
    }
    
    void sleepMs(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

public:
    bool load(const std::vector<uint8_t>& binary) {
        if (binary.size() < sizeof(TuiHeader)) {
            std::cerr << "Error: Invalid TUI file - too small\n";
            return false;
        }
        
        // Read header
        TuiHeader header;
        memcpy(&header, binary.data(), sizeof(header));
        
        // Verify magic
        if (memcmp(header.magic, TUI_MAGIC, 8) != 0) {
            std::cerr << "Error: Invalid TUI file - bad magic\n";
            return false;
        }
        
        // Verify version
        if (header.version != TUI_VERSION) {
            std::cerr << "Error: Unsupported TUI version: " << header.version << "\n";
            return false;
        }
        
        // Read function table
        size_t offset = sizeof(TuiHeader);
        functions.resize(header.numFunctions);
        for (uint32_t i = 0; i < header.numFunctions; i++) {
            memcpy(&functions[i], binary.data() + offset, sizeof(TuiFunctionEntry));
            offset += sizeof(TuiFunctionEntry);
        }
        
        // Read string offsets
        std::vector<uint32_t> stringOffsets(header.numStrings);
        for (uint32_t i = 0; i < header.numStrings; i++) {
            stringOffsets[i] = binary[offset] | (binary[offset+1] << 8) |
                               (binary[offset+2] << 16) | (binary[offset+3] << 24);
            offset += 4;
        }
        
        // Read bytecode
        bytecode.assign(binary.begin() + offset, binary.begin() + offset + header.bytecodeSize);
        offset += header.bytecodeSize;
        
        // Read string data
        std::vector<char> stringData(binary.begin() + offset, binary.end());
        
        // Parse strings
        strings.resize(header.numStrings);
        for (uint32_t i = 0; i < header.numStrings; i++) {
            strings[i] = std::string(stringData.data() + stringOffsets[i]);
        }
        
        // Verify checksum
        uint64_t checksum = 0;
        for (uint8_t b : bytecode) {
            checksum = checksum * 31 + b;
        }
        for (char c : stringData) {
            checksum = checksum * 31 + (uint8_t)c;
        }
        if (checksum != header.checksum) {
            std::cerr << "Warning: TUI checksum mismatch - file may be corrupted\n";
        }
        
        entryPoint = header.entryPoint;
        return true;
    }
    
    int run() {
        ip = entryPoint;
        running = true;
        locals.resize(256); // Pre-allocate local variables
        
        while (running && ip < bytecode.size()) {
            TuiOpcode op = static_cast<TuiOpcode>(readByte());
            
            switch (op) {
                case TuiOpcode::NOP:
                    break;
                    
                case TuiOpcode::PUSH_INT:
                    push(TuiValue(readInt32()));
                    break;
                    
                case TuiOpcode::PUSH_FLOAT:
                    push(TuiValue(readFloat()));
                    break;
                    
                case TuiOpcode::PUSH_STR: {
                    uint32_t idx = readUint32();
                    if (idx < strings.size()) {
                        push(TuiValue(strings[idx]));
                    } else {
                        push(TuiValue(""));
                    }
                    break;
                }
                    
                case TuiOpcode::PUSH_BOOL:
                    push(TuiValue(readByte() != 0));
                    break;
                    
                case TuiOpcode::POP:
                    pop();
                    break;
                    
                case TuiOpcode::DUP: {
                    TuiValue v = pop();
                    push(v);
                    push(v);
                    break;
                }
                    
                case TuiOpcode::SWAP: {
                    TuiValue a = pop();
                    TuiValue b = pop();
                    push(a);
                    push(b);
                    break;
                }
                    
                case TuiOpcode::ADD: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    if (a.type == TuiValue::STRING || b.type == TuiValue::STRING) {
                        push(TuiValue(a.toString() + b.toString()));
                    } else if (a.type == TuiValue::FLOAT || b.type == TuiValue::FLOAT) {
                        push(TuiValue(a.toFloat() + b.toFloat()));
                    } else {
                        push(TuiValue(a.toInt() + b.toInt()));
                    }
                    break;
                }
                    
                case TuiOpcode::SUB: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    if (a.type == TuiValue::FLOAT || b.type == TuiValue::FLOAT) {
                        push(TuiValue(a.toFloat() - b.toFloat()));
                    } else {
                        push(TuiValue(a.toInt() - b.toInt()));
                    }
                    break;
                }
                    
                case TuiOpcode::MUL: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    if (a.type == TuiValue::FLOAT || b.type == TuiValue::FLOAT) {
                        push(TuiValue(a.toFloat() * b.toFloat()));
                    } else {
                        push(TuiValue(a.toInt() * b.toInt()));
                    }
                    break;
                }
                    
                case TuiOpcode::DIV: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    if (b.toFloat() == 0) {
                        std::cerr << "Runtime error: Division by zero\n";
                        push(TuiValue(0));
                    } else if (a.type == TuiValue::FLOAT || b.type == TuiValue::FLOAT) {
                        push(TuiValue(a.toFloat() / b.toFloat()));
                    } else {
                        push(TuiValue(a.toInt() / b.toInt()));
                    }
                    break;
                }
                    
                case TuiOpcode::MOD: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    if (b.toInt() == 0) {
                        std::cerr << "Runtime error: Modulo by zero\n";
                        push(TuiValue(0));
                    } else {
                        push(TuiValue(a.toInt() % b.toInt()));
                    }
                    break;
                }
                    
                case TuiOpcode::NEG: {
                    TuiValue a = pop();
                    if (a.type == TuiValue::FLOAT) {
                        push(TuiValue(-a.floatVal));
                    } else {
                        push(TuiValue(-a.toInt()));
                    }
                    break;
                }
                    
                case TuiOpcode::EQ: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toString() == b.toString()));
                    break;
                }
                    
                case TuiOpcode::NE: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toString() != b.toString()));
                    break;
                }
                    
                case TuiOpcode::LT: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toFloat() < b.toFloat()));
                    break;
                }
                    
                case TuiOpcode::GT: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toFloat() > b.toFloat()));
                    break;
                }
                    
                case TuiOpcode::LE: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toFloat() <= b.toFloat()));
                    break;
                }
                    
                case TuiOpcode::GE: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toFloat() >= b.toFloat()));
                    break;
                }
                    
                case TuiOpcode::AND: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toBool() && b.toBool()));
                    break;
                }
                    
                case TuiOpcode::OR: {
                    TuiValue b = pop();
                    TuiValue a = pop();
                    push(TuiValue(a.toBool() || b.toBool()));
                    break;
                }
                    
                case TuiOpcode::NOT: {
                    TuiValue a = pop();
                    push(TuiValue(!a.toBool()));
                    break;
                }
                    
                case TuiOpcode::JMP:
                    ip = readUint32();
                    break;
                    
                case TuiOpcode::JZ: {
                    uint32_t target = readUint32();
                    if (!pop().toBool()) {
                        ip = target;
                    }
                    break;
                }
                    
                case TuiOpcode::JNZ: {
                    uint32_t target = readUint32();
                    if (pop().toBool()) {
                        ip = target;
                    }
                    break;
                }
                    
                case TuiOpcode::CALL: {
                    uint32_t target = readUint32();
                    uint8_t numArgs = readByte();
                    callStack.push(ip);
                    frameStack.push(locals.size());
                    ip = target;
                    break;
                }
                    
                case TuiOpcode::RET: {
                    if (callStack.empty()) {
                        running = false;
                    } else {
                        ip = callStack.top();
                        callStack.pop();
                        if (!frameStack.empty()) {
                            frameStack.pop();
                        }
                    }
                    break;
                }
                    
                case TuiOpcode::LOAD: {
                    uint32_t slot = readUint32();
                    if (slot < locals.size()) {
                        push(locals[slot]);
                    } else {
                        push(TuiValue(0));
                    }
                    break;
                }
                    
                case TuiOpcode::STORE: {
                    uint32_t slot = readUint32();
                    if (slot >= locals.size()) {
                        locals.resize(slot + 1);
                    }
                    locals[slot] = pop();
                    break;
                }
                    
                case TuiOpcode::PRINT: {
                    TuiValue v = pop();
                    std::cout << v.toString();
                    std::cout.flush();
                    break;
                }
                    
                case TuiOpcode::PRINTLN: {
                    TuiValue v = pop();
                    std::cout << v.toString() << std::endl;
                    break;
                }
                    
                case TuiOpcode::INPUT: {
                    std::string line;
                    std::getline(std::cin, line);
                    push(TuiValue(line));
                    break;
                }
                    
                case TuiOpcode::INPUT_INT: {
                    int val;
                    std::cin >> val;
                    push(TuiValue(val));
                    break;
                }
                    
                case TuiOpcode::INPUT_FLOAT: {
                    float val;
                    std::cin >> val;
                    push(TuiValue(val));
                    break;
                }
                    
                case TuiOpcode::CLEAR:
                    clearScreen();
                    break;
                    
                case TuiOpcode::GOTOXY: {
                    TuiValue y = pop();
                    TuiValue x = pop();
                    gotoXY(x.toInt(), y.toInt());
                    break;
                }
                    
                case TuiOpcode::SET_COLOR: {
                    TuiValue color = pop();
                    setColor(color.toInt());
                    break;
                }
                    
                case TuiOpcode::GET_KEY:
                    push(TuiValue(getKey()));
                    break;
                    
                case TuiOpcode::SLEEP: {
                    TuiValue ms = pop();
                    sleepMs(ms.toInt());
                    break;
                }
                    
                case TuiOpcode::HALT:
                    running = false;
                    break;
                    
                default:
                    std::cerr << "Unknown opcode: 0x" << std::hex << (int)op << std::dec << "\n";
                    running = false;
                    break;
            }
        }
        
        // Return value from stack if available
        if (!stack.empty()) {
            return pop().toInt();
        }
        return 0;
    }
    
    // Load and run from file
    static int runFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Cannot open TUI file: " << path << "\n";
            return 1;
        }
        
        std::vector<uint8_t> binary((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
        file.close();
        
        TuiRuntime runtime;
        if (!runtime.load(binary)) {
            return 1;
        }
        
        return runtime.run();
    }
};

} // namespace disp

#endif // DISPLEXITY_TUIRUNTIME_H
