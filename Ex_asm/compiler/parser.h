#ifndef DISPLEXITY_PARSER_H
#define DISPLEXITY_PARSER_H

#include "lexer.h"
#include "ast.h"
#include <stdexcept>
#include <iostream>

namespace disp {

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;

    const Token& peek() const {
        if (current >= tokens.size()) return tokens.back();
        return tokens[current];
    }

    const Token& peekNext() const {
        if (current + 1 >= tokens.size()) return tokens.back();
        return tokens[current + 1];
    }

    Token advance() {
        return tokens[current++];
    }

    bool check(TokenType type) const {
        return peek().type == type;
    }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    void expect(TokenType type, const std::string& msg) {
        if (!check(type)) {
            throw std::runtime_error(msg + " at line " + std::to_string(peek().line));
        }
        advance();
    }

public:
    Parser(const std::vector<Token>& toks) : tokens(toks) {}

    Program parse() {
        Program prog;
        while (!check(TokenType::END_OF_FILE)) {
            try {
                auto stmt = parseStatement();
                if (stmt) prog.statements.push_back(stmt);
            } catch (const std::exception& e) {
                std::cerr << "Parse error: " << e.what() << "\n";
                advance();
            }
        }
        return prog;
    }

private:
    StmtPtr parseStatement() {
        if (match(TokenType::PRINT)) {
            return parsePrint();
        } else if (match(TokenType::IF)) {
            return parseIf();
        } else if (match(TokenType::WHILE)) {
            return parseWhile();
        } else if (match(TokenType::FOR)) {
            return parseFor();
        } else if (match(TokenType::FUNC)) {
            return parseFunction();
        } else if (match(TokenType::CLASS)) {
            return parseClass();
        } else if (match(TokenType::RETURN)) {
            return parseReturn();
        } else if (check(TokenType::IDENT)) {
            return parseAssignmentOrExpr();
        }
        if (check(TokenType::SEMICOLON)) advance();
        return nullptr;
    }

    StmtPtr parsePrint() {
        auto expr = parseExpression();
        match(TokenType::SEMICOLON);
        return std::make_shared<PrintStmt>(expr);
    }

    StmtPtr parseIf() {
        expect(TokenType::LPAREN, "Expected '(' after if");
        auto cond = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after condition");
        auto thenBlock = parseBlock();
        std::vector<StmtPtr> elseBlock;
        if (match(TokenType::ELSE)) {
            elseBlock = parseBlock();
        }
        return std::make_shared<IfStmt>(cond, thenBlock, elseBlock);
    }

    StmtPtr parseWhile() {
        expect(TokenType::LPAREN, "Expected '(' after while");
        auto cond = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after condition");
        auto body = parseBlock();
        return std::make_shared<WhileStmt>(cond, body);
    }

    StmtPtr parseFor() {
        expect(TokenType::LPAREN, "Expected '(' after for");
        std::string var = peek().lexeme;
        expect(TokenType::IDENT, "Expected variable name");
        expect(TokenType::EQ, "Expected '=' in for loop");
        auto start = parseExpression();
        expect(TokenType::SEMICOLON, "Expected ';' in for loop");
        auto end = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after for");
        auto body = parseBlock();
        return std::make_shared<ForStmt>(var, start, end, body);
    }

    StmtPtr parseFunction() {
        std::string name = peek().lexeme;
        expect(TokenType::IDENT, "Expected function name");
        
        std::vector<std::pair<std::string, std::string>> params;
        expect(TokenType::LPAREN, "Expected '(' after function name");
        while (!check(TokenType::RPAREN)) {
            std::string pname = peek().lexeme;
            expect(TokenType::IDENT, "Expected parameter name");
            if (match(TokenType::COLON)) {
                std::string ptype = peek().lexeme;
                expect(TokenType::IDENT, "Expected type");
                params.push_back({pname, ptype});
            } else {
                params.push_back({pname, "int"});
            }
            if (!check(TokenType::RPAREN)) expect(TokenType::COMMA, "Expected ','");
        }
        expect(TokenType::RPAREN, "Expected ')'");
        
        std::string returnType = "int";
        if (match(TokenType::ARROW)) {
            returnType = peek().lexeme;
            expect(TokenType::IDENT, "Expected return type");
        }
        
        auto body = parseBlock();
        return std::make_shared<FunctionDecl>(name, params, returnType, body);
    }

    StmtPtr parseClass() {
        std::string name = peek().lexeme;
        expect(TokenType::IDENT, "Expected class name");
        auto cls = std::make_shared<ClassDecl>(name);
        
        expect(TokenType::LBRACE, "Expected '{' after class name");
        while (!check(TokenType::RBRACE)) {
            if (check(TokenType::IDENT)) {
                std::string fname = peek().lexeme;
                advance();
                std::string ftype = "int";
                if (match(TokenType::COLON)) {
                    ftype = peek().lexeme;
                    expect(TokenType::IDENT, "Expected type");
                }
                cls->fields.push_back({fname, ftype});
                match(TokenType::SEMICOLON);
            } else {
                advance();
            }
        }
        expect(TokenType::RBRACE, "Expected '}'");
        return cls;
    }

    StmtPtr parseReturn() {
        ExprPtr value = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            value = parseExpression();
        }
        match(TokenType::SEMICOLON);
        return std::make_shared<ReturnStmt>(value);
    }

    StmtPtr parseAssignmentOrExpr() {
        std::string name = peek().lexeme;
        advance();
        
        if (match(TokenType::LBRACKET)) {
            auto index = parseExpression();
            expect(TokenType::RBRACKET, "Expected ']'");
            if (match(TokenType::EQ)) {
                auto value = parseExpression();
                match(TokenType::SEMICOLON);
                return std::make_shared<ArrayAssignment>(name, index, value);
            }
        } else if (match(TokenType::EQ)) {
            auto value = parseExpression();
            match(TokenType::SEMICOLON);
            return std::make_shared<Assignment>(name, value);
        }
        match(TokenType::SEMICOLON);
        return nullptr;
    }

    std::vector<StmtPtr> parseBlock() {
        std::vector<StmtPtr> stmts;
        expect(TokenType::LBRACE, "Expected '{'");
        while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
            auto stmt = parseStatement();
            if (stmt) stmts.push_back(stmt);
        }
        expect(TokenType::RBRACE, "Expected '}'");
        return stmts;
    }

    ExprPtr parseExpression() {
        return parseLogicalOr();
    }

    ExprPtr parseLogicalOr() {
        auto left = parseLogicalAnd();
        while (check(TokenType::LOR)) {
            std::string op = peek().lexeme;
            advance();
            auto right = parseLogicalAnd();
            left = std::make_shared<BinaryOp>(left, op, right);
        }
        return left;
    }

    ExprPtr parseLogicalAnd() {
        auto left = parseEquality();
        while (check(TokenType::LAND)) {
            std::string op = peek().lexeme;
            advance();
            auto right = parseEquality();
            left = std::make_shared<BinaryOp>(left, op, right);
        }
        return left;
    }

    ExprPtr parseEquality() {
        auto left = parseComparison();
        while (check(TokenType::EQEQ) || check(TokenType::NEQ)) {
            std::string op = peek().lexeme;
            advance();
            auto right = parseComparison();
            left = std::make_shared<BinaryOp>(left, op, right);
        }
        return left;
    }

    ExprPtr parseComparison() {
        auto left = parseAddition();
        while (check(TokenType::LT) || check(TokenType::LE) || check(TokenType::GT) || check(TokenType::GE)) {
            std::string op = peek().lexeme;
            advance();
            auto right = parseAddition();
            left = std::make_shared<BinaryOp>(left, op, right);
        }
        return left;
    }

    ExprPtr parseAddition() {
        auto left = parseMultiplication();
        while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
            std::string op = peek().lexeme;
            advance();
            auto right = parseMultiplication();
            left = std::make_shared<BinaryOp>(left, op, right);
        }
        return left;
    }

    ExprPtr parseMultiplication() {
        auto left = parseUnary();
        while (check(TokenType::STAR) || check(TokenType::SLASH) || check(TokenType::PERCENT)) {
            std::string op = peek().lexeme;
            advance();
            auto right = parseUnary();
            left = std::make_shared<BinaryOp>(left, op, right);
        }
        return left;
    }

    ExprPtr parseUnary() {
        if (check(TokenType::NOT) || check(TokenType::MINUS)) {
            std::string op = peek().lexeme;
            advance();
            return std::make_shared<UnaryOp>(op, parseUnary());
        }
        return parsePrimary();
    }

    ExprPtr parsePrimary() {
        if (match(TokenType::INT)) {
            return std::make_shared<IntLiteral>(std::stol(tokens[current - 1].lexeme));
        }
        if (match(TokenType::STRING)) {
            return std::make_shared<StringLiteral>(tokens[current - 1].lexeme);
        }
        if (match(TokenType::TRUE)) {
            return std::make_shared<BoolLiteral>(true);
        }
        if (match(TokenType::FALSE)) {
            return std::make_shared<BoolLiteral>(false);
        }
        if (check(TokenType::IDENT)) {
            std::string name = peek().lexeme;
            advance();
            
            if (match(TokenType::LBRACKET)) {
                auto index = parseExpression();
                expect(TokenType::RBRACKET, "Expected ']'");
                return std::make_shared<ArrayAccess>(name, index);
            }
            
            if (match(TokenType::LPAREN)) {
                std::vector<ExprPtr> args;
                while (!check(TokenType::RPAREN)) {
                    args.push_back(parseExpression());
                    if (!check(TokenType::RPAREN)) expect(TokenType::COMMA, "Expected ','");
                }
                expect(TokenType::RPAREN, "Expected ')'");
                return std::make_shared<FunctionCall>(name, args);
            }
            
            return std::make_shared<Variable>(name);
        }
        if (match(TokenType::LPAREN)) {
            auto expr = parseExpression();
            expect(TokenType::RPAREN, "Expected ')'");
            return expr;
        }
        if (match(TokenType::ARRAY)) {
            expect(TokenType::LBRACKET, "Expected '[' after Array");
            std::vector<ExprPtr> elements;
            while (!check(TokenType::RBRACKET)) {
                elements.push_back(parseExpression());
                if (!check(TokenType::RBRACKET)) expect(TokenType::COMMA, "Expected ','");
            }
            expect(TokenType::RBRACKET, "Expected ']'");
            return std::make_shared<ArrayLiteral>(elements);
        }
        throw std::runtime_error("Unexpected token: " + peek().lexeme);
    }
};

} // namespace disp

#endif // DISPLEXITY_PARSER_H
