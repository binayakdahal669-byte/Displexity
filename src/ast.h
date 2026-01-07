#ifndef DISPLEXITY_AST_H
#define DISPLEXITY_AST_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace disp {

// Forward declarations
class Stmt;
class Expr;

using StmtPtr = std::shared_ptr<Stmt>;
using ExprPtr = std::shared_ptr<Expr>;

// Base expression class
class Expr {
public:
    virtual ~Expr() = default;
};

// Base statement class
class Stmt {
public:
    virtual ~Stmt() = default;
};

// Literals
class IntLiteral : public Expr {
public:
    long value;
    IntLiteral(long v) : value(v) {}
};

class FloatLiteral : public Expr {
public:
    double value;
    FloatLiteral(double v) : value(v) {}
};

class StringLiteral : public Expr {
public:
    std::string value;
    StringLiteral(const std::string& v) : value(v) {}
};

class BoolLiteral : public Expr {
public:
    bool value;
    BoolLiteral(bool v) : value(v) {}
};

class ArrayLiteral : public Expr {
public:
    std::vector<ExprPtr> elements;
    ArrayLiteral(const std::vector<ExprPtr>& e) : elements(e) {}
};

// Variables and identifiers
class Variable : public Expr {
public:
    std::string name;
    Variable(const std::string& n) : name(n) {}
};

class ArrayAccess : public Expr {
public:
    std::string name;
    ExprPtr index;
    ArrayAccess(const std::string& n, ExprPtr i) : name(n), index(i) {}
};

// Binary operations
class BinaryOp : public Expr {
public:
    ExprPtr left, right;
    std::string op;
    BinaryOp(ExprPtr l, const std::string& o, ExprPtr r) : left(l), op(o), right(r) {}
};

// Unary operations
class UnaryOp : public Expr {
public:
    ExprPtr expr;
    std::string op;
    UnaryOp(const std::string& o, ExprPtr e) : expr(e), op(o) {}
};

// Function call
class FunctionCall : public Expr {
public:
    std::string name;
    std::vector<ExprPtr> args;
    FunctionCall(const std::string& n, const std::vector<ExprPtr>& a) : name(n), args(a) {}
};

// Assignment
class Assignment : public Stmt {
public:
    std::string name;
    ExprPtr value;
    Assignment(const std::string& n, ExprPtr v) : name(n), value(v) {}
};

// Array assignment
class ArrayAssignment : public Stmt {
public:
    std::string name;
    ExprPtr index;
    ExprPtr value;
    ArrayAssignment(const std::string& n, ExprPtr i, ExprPtr v) : name(n), index(i), value(v) {}
};

// Stream expression for print: "text" << x << "more"
class StreamExpr : public Expr {
public:
    std::vector<ExprPtr> parts;  // alternating strings and expressions
    StreamExpr(const std::vector<ExprPtr>& p) : parts(p) {}
};

// Format conversion expression: x >> 0x (returns converted value)
class FormatConvert : public Expr {
public:
    std::string varName;
    std::string format;  // "bin", "hex", "oct", "dec"
    FormatConvert(const std::string& v, const std::string& f) : varName(v), format(f) {}
};

// Print statement - now supports stream syntax
class PrintStmt : public Stmt {
public:
    ExprPtr expr;
    bool isStream;  // true if using stream syntax print(...)
    PrintStmt(ExprPtr e, bool stream = false) : expr(e), isStream(stream) {}
};

// Input statement - Python style: input(var, "prompt")
class InputStmt : public Stmt {
public:
    std::string prompt;
    std::string resultVar;
    std::string varType;  // "str", "int", "float", "bool"
    InputStmt(const std::string& p, const std::string& r, const std::string& t = "str") 
        : prompt(p), resultVar(r), varType(t) {}
};

// Variable declaration: int x; float y; str z;
class VarDecl : public Stmt {
public:
    std::string name;
    std::string varType;  // "int", "float", "str", "bool"
    ExprPtr initialValue;  // optional
    VarDecl(const std::string& t, const std::string& n, ExprPtr init = nullptr) 
        : name(n), varType(t), initialValue(init) {}
};

// Type conversion: float << x; int << y; str << z;
class TypeConvert : public Stmt {
public:
    std::string targetType;  // "int", "float", "str"
    std::string varName;
    TypeConvert(const std::string& t, const std::string& v) : targetType(t), varName(v) {}
};

// Format output: x >> 0b; x >> 0x; x >> 0o; x >> 0d;
class FormatOutput : public Stmt {
public:
    std::string varName;
    std::string format;  // "bin", "hex", "oct", "dec"
    FormatOutput(const std::string& v, const std::string& f) : varName(v), format(f) {}
};

// If statement
class IfStmt : public Stmt {
public:
    ExprPtr condition;
    std::vector<StmtPtr> thenBlock;
    std::vector<StmtPtr> elseBlock;
    IfStmt(ExprPtr cond, const std::vector<StmtPtr>& tb, const std::vector<StmtPtr>& eb = {})
        : condition(cond), thenBlock(tb), elseBlock(eb) {}
};

// While loop
class WhileStmt : public Stmt {
public:
    ExprPtr condition;
    std::vector<StmtPtr> body;
    WhileStmt(ExprPtr cond, const std::vector<StmtPtr>& b) : condition(cond), body(b) {}
};

// For loop
class ForStmt : public Stmt {
public:
    std::string var;
    ExprPtr start, end;
    std::vector<StmtPtr> body;
    ForStmt(const std::string& v, ExprPtr s, ExprPtr e, const std::vector<StmtPtr>& b)
        : var(v), start(s), end(e), body(b) {}
};

// Function declaration
class FunctionDecl : public Stmt {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> params; // (name, type)
    std::string returnType;
    std::vector<StmtPtr> body;
    FunctionDecl(const std::string& n, const std::vector<std::pair<std::string, std::string>>& p,
                 const std::string& rt, const std::vector<StmtPtr>& b)
        : name(n), params(p), returnType(rt), body(b) {}
};

// Return statement
class ReturnStmt : public Stmt {
public:
    ExprPtr value;
    ReturnStmt(ExprPtr v = nullptr) : value(v) {}
};

// HTTP Request statement
class HttpStmt : public Stmt {
public:
    ExprPtr url;
    std::string method;
    std::string resultVar;
    HttpStmt(ExprPtr u, const std::string& m, const std::string& r) 
        : url(u), method(m), resultVar(r) {}
};

// Include statement
class IncludeStmt : public Stmt {
public:
    std::string path;
    IncludeStmt(const std::string& p) : path(p) {}
};

// Graphics statements
class GraphicsStmt : public Stmt {
public:
    std::string command;
    std::vector<ExprPtr> args;
    GraphicsStmt(const std::string& cmd, const std::vector<ExprPtr>& a = {}) 
        : command(cmd), args(a) {}
};

// Keypress statement
class KeypressStmt : public Stmt {
public:
    std::string resultVar;
    KeypressStmt(const std::string& r) : resultVar(r) {}
};

// Hotkey registration
class HotkeyStmt : public Stmt {
public:
    ExprPtr key;
    std::string callback;
    HotkeyStmt(ExprPtr k, const std::string& cb) : key(k), callback(cb) {}
};

// AI query statement
class AIStmt : public Stmt {
public:
    ExprPtr prompt;
    std::string resultVar;
    AIStmt(ExprPtr p, const std::string& r) : prompt(p), resultVar(r) {}
};

// Class declaration
class ClassDecl : public Stmt {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields; // (name, type)
    std::map<std::string, StmtPtr> methods;
    ClassDecl(const std::string& n) : name(n) {}
};

// Icon setting statement: for_this_use_icon("path.ico")
class IconStmt : public Stmt {
public:
    std::string iconPath;
    IconStmt(const std::string& p) : iconPath(p) {}
};

// Image loading expression: load_image("path", x, y, w, h, sx, sy, sw, sh)
class LoadImageExpr : public Expr {
public:
    std::string path;
    std::vector<ExprPtr> args;  // x, y, w, h, sx, sy, sw, sh (like ctx.drawImage 9-arg)
    LoadImageExpr(const std::string& p, const std::vector<ExprPtr>& a) : path(p), args(a) {}
};

// Sound loading expression: load_sound("path"), play_sound(id), stop_sound(id)
class SoundExpr : public Expr {
public:
    std::string command;  // "load", "play", "stop", "pause", "volume"
    std::string path;
    std::vector<ExprPtr> args;
    SoundExpr(const std::string& cmd, const std::string& p = "", const std::vector<ExprPtr>& a = {}) 
        : command(cmd), path(p), args(a) {}
};

// From-include statement: from <path> include funcname
class FromIncludeStmt : public Stmt {
public:
    std::string path;
    std::vector<std::string> symbols;  // functions/symbols to import
    FromIncludeStmt(const std::string& p, const std::vector<std::string>& s) : path(p), symbols(s) {}
};

// Program (root)
class Program {
public:
    std::vector<StmtPtr> statements;
};

} // namespace disp

#endif // DISPLEXITY_AST_H
