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

// Print statement
class PrintStmt : public Stmt {
public:
    ExprPtr expr;
    PrintStmt(ExprPtr e) : expr(e) {}
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

// Class declaration
class ClassDecl : public Stmt {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields; // (name, type)
    std::map<std::string, StmtPtr> methods;
    ClassDecl(const std::string& n) : name(n) {}
};

// Program (root)
class Program {
public:
    std::vector<StmtPtr> statements;
};

} // namespace disp

#endif // DISPLEXITY_AST_H
