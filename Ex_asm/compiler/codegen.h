#ifndef DISPLEXITY_CODEGEN_H
#define DISPLEXITY_CODEGEN_H

#include "ast.h"
#include <sstream>
#include <map>
#include <set>

namespace disp {

class CodeGenerator {
private:
    std::stringstream code;
    std::map<std::string, std::string> variables;
    std::set<std::string> declaredVars;
    int tmpCounter = 0;

    std::string genTmp() {
        return "__tmp_" + std::to_string(tmpCounter++);
    }

public:
    std::string generate(const Program& prog) {
        code.str("");
        code << "#include <stdio.h>\n";
        code << "#include <string.h>\n";
        code << "#include <stdlib.h>\n";
        code << "\n";

        // Pre-declare functions and classes
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                code << func->returnType << " " << func->name << "(";
                for (size_t i = 0; i < func->params.size(); i++) {
                    code << func->params[i].second << " " << func->params[i].first;
                    if (i < func->params.size() - 1) code << ", ";
                }
                code << ");\n";
            }
        }
        code << "\n";

        // Emit functions and main
        bool hasMain = false;
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                emitFunction(func);
            } else {
                hasMain = true;
            }
        }

        // Main function
        if (hasMain) {
            code << "\nint main() {\n";
            for (const auto& stmt : prog.statements) {
                if (!std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                    emitStatement(stmt);
                }
            }
            code << "    return 0;\n";
            code << "}\n";
        }

        return code.str();
    }

private:
    void emitFunction(std::shared_ptr<FunctionDecl> func) {
        code << func->returnType << " " << func->name << "(";
        for (size_t i = 0; i < func->params.size(); i++) {
            code << func->params[i].second << " " << func->params[i].first;
            if (i < func->params.size() - 1) code << ", ";
        }
        code << ") {\n";
        
        declaredVars.clear();
        for (const auto& stmt : func->body) {
            emitStatement(stmt);
        }
        
        code << "    return 0;\n";
        code << "}\n\n";
    }

    void emitStatement(const StmtPtr& stmt) {
        if (auto print = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            code << "    printf(\"%s\\n\", ";
            emitExprAsString(print->expr);
            code << ");\n";
        } else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            if (declaredVars.find(assign->name) == declaredVars.end()) {
                code << "    int " << assign->name << " = ";
                declaredVars.insert(assign->name);
            } else {
                code << "    " << assign->name << " = ";
            }
            emitExpression(assign->value);
            code << ";\n";
        } else if (auto arrAssign = std::dynamic_pointer_cast<ArrayAssignment>(stmt)) {
            code << "    " << arrAssign->name << "[";
            emitExpression(arrAssign->index);
            code << "] = ";
            emitExpression(arrAssign->value);
            code << ";\n";
        } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            code << "    if (";
            emitExprAsInt(ifStmt->condition);
            code << ") {\n";
            for (const auto& s : ifStmt->thenBlock) emitStatement(s);
            code << "    }";
            if (!ifStmt->elseBlock.empty()) {
                code << " else {\n";
                for (const auto& s : ifStmt->elseBlock) emitStatement(s);
                code << "    }";
            }
            code << "\n";
        } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            code << "    while (";
            emitExprAsInt(whileStmt->condition);
            code << ") {\n";
            for (const auto& s : whileStmt->body) emitStatement(s);
            code << "    }\n";
        } else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            code << "    for (int " << forStmt->var << " = ";
            emitExpression(forStmt->start);
            code << "; " << forStmt->var << " < ";
            emitExpression(forStmt->end);
            code << "; " << forStmt->var << "++) {\n";
            for (const auto& s : forStmt->body) emitStatement(s);
            code << "    }\n";
        } else if (auto retStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
            code << "    return";
            if (retStmt->value) {
                code << " ";
                emitExpression(retStmt->value);
            }
            code << ";\n";
        }
    }

    void emitExpression(const ExprPtr& expr) {
        if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(expr)) {
            code << intLit->value;
        } else if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(expr)) {
            code << "\"" << escapeString(strLit->value) << "\"";
        } else if (auto boolLit = std::dynamic_pointer_cast<BoolLiteral>(expr)) {
            code << (boolLit->value ? "1" : "0");
        } else if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            code << var->name;
        } else if (auto arrAccess = std::dynamic_pointer_cast<ArrayAccess>(expr)) {
            code << arrAccess->name << "[";
            emitExpression(arrAccess->index);
            code << "]";
        } else if (auto binOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
            code << "(";
            emitExpression(binOp->left);
            code << " " << binOp->op << " ";
            emitExpression(binOp->right);
            code << ")";
        } else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
            code << unaryOp->op << "(";
            emitExpression(unaryOp->expr);
            code << ")";
        } else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
            code << funcCall->name << "(";
            for (size_t i = 0; i < funcCall->args.size(); i++) {
                emitExpression(funcCall->args[i]);
                if (i < funcCall->args.size() - 1) code << ", ";
            }
            code << ")";
        } else if (auto arrLit = std::dynamic_pointer_cast<ArrayLiteral>(expr)) {
            code << "{";
            for (size_t i = 0; i < arrLit->elements.size(); i++) {
                emitExpression(arrLit->elements[i]);
                if (i < arrLit->elements.size() - 1) code << ", ";
            }
            code << "}";
        }
    }

    void emitExprAsInt(const ExprPtr& expr) {
        emitExpression(expr);
    }

    void emitExprAsString(const ExprPtr& expr) {
        if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(expr)) {
            code << "\"" << escapeString(strLit->value) << "\"";
        } else if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            // Format int as string
            code << "({ char __buf[64]; snprintf(__buf, 64, \"%d\", " << var->name << "); __buf; })";
        } else {
            emitExpression(expr);
        }
    }

    std::string escapeString(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else if (c == '\t') result += "\\t";
            else result += c;
        }
        return result;
    }
};

} // namespace disp

#endif // DISPLEXITY_CODEGEN_H
