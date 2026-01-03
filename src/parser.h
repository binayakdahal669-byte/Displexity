#ifndef DISPLEXITY_PARSER_H
#define DISPLEXITY_PARSER_H

#include "lexer.h"
#include "ast.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace disp {

struct ParseError {
    std::string message;
    int line;
    int column;
    std::string token;
    std::string expected;
};

class Parser {
private:
    std::vector<Token> tokens;
    std::vector<std::string> sourceLines;
    size_t current = 0;
    std::vector<ParseError> errors;
    bool panicMode = false;

    const Token& peek() const {
        if (current >= tokens.size()) return tokens.back();
        return tokens[current];
    }

    const Token& peekNext() const {
        if (current + 1 >= tokens.size()) return tokens.back();
        return tokens[current + 1];
    }

    const Token& previous() const {
        if (current == 0) return tokens[0];
        return tokens[current - 1];
    }

    Token advance() {
        if (!check(TokenType::END_OF_FILE)) current++;
        return previous();
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

    void addError(const std::string& msg, const std::string& expected = "") {
        if (panicMode) return;
        panicMode = true;
        
        ParseError err;
        err.message = msg;
        err.line = peek().line;
        err.column = peek().column;
        err.token = peek().lexeme;
        err.expected = expected;
        errors.push_back(err);
    }

    void synchronize() {
        panicMode = false;
        advance();
        
        while (!check(TokenType::END_OF_FILE)) {
            if (previous().type == TokenType::SEMICOLON) return;
            if (previous().type == TokenType::RBRACE) return;
            
            switch (peek().type) {
                case TokenType::FUNC:
                case TokenType::CLASS:
                case TokenType::IF:
                case TokenType::WHILE:
                case TokenType::FOR:
                case TokenType::PRINT:
                case TokenType::RETURN:
                    return;
                default:
                    break;
            }
            advance();
        }
    }

    bool expect(TokenType type, const std::string& msg) {
        if (check(type)) {
            advance();
            return true;
        }
        addError(msg, tokenTypeName(type));
        return false;
    }

    std::string tokenTypeName(TokenType type) {
        switch (type) {
            case TokenType::LPAREN: return "'('";
            case TokenType::RPAREN: return "')'";
            case TokenType::LBRACE: return "'{'";
            case TokenType::RBRACE: return "'}'";
            case TokenType::LBRACKET: return "'['";
            case TokenType::RBRACKET: return "']'";
            case TokenType::SEMICOLON: return "';'";
            case TokenType::COMMA: return "','";
            case TokenType::EQ: return "'='";
            case TokenType::IDENT: return "identifier";
            case TokenType::INT: return "number";
            case TokenType::STRING: return "string";
            default: return "token";
        }
    }

public:
    Parser(const std::vector<Token>& toks) : tokens(toks) {}
    
    void setSourceLines(const std::vector<std::string>& lines) {
        sourceLines = lines;
    }

    Program parse() {
        Program prog;
        while (!check(TokenType::END_OF_FILE)) {
            try {
                auto stmt = parseStatement();
                if (stmt) prog.statements.push_back(stmt);
            } catch (const std::exception& e) {
                addError(e.what());
                synchronize();
            }
        }
        return prog;
    }

    bool hasErrors() const { return !errors.empty(); }
    
    const std::vector<ParseError>& getErrors() const { return errors; }
    
    void printErrors(std::ostream& out = std::cerr) const {
        for (const auto& err : errors) {
            out << "\033[1;31mSyntax Error\033[0m at line " << err.line << ", column " << err.column << ":\n";
            out << "  " << err.message << "\n";
            if (!err.token.empty()) {
                out << "  Found: '\033[1;33m" << err.token << "\033[0m'";
                if (!err.expected.empty()) {
                    out << ", expected: \033[1;32m" << err.expected << "\033[0m";
                }
                out << "\n";
            }
            // Show source line if available
            if (err.line > 0 && err.line <= (int)sourceLines.size()) {
                out << "\n  " << sourceLines[err.line - 1] << "\n";
                out << "  ";
                for (int i = 1; i < err.column; i++) {
                    out << (sourceLines[err.line - 1][i-1] == '\t' ? '\t' : ' ');
                }
                out << "\033[1;31m^\033[0m\n";
            }
            out << "\n";
        }
    }

private:
    StmtPtr parseStatement() {
        if (match(TokenType::PRINT)) {
            return parsePrint();
        } else if (match(TokenType::INPUT)) {
            return parseInput();
        } else if (match(TokenType::TYPE_INT) || match(TokenType::TYPE_FLOAT) || 
                   match(TokenType::TYPE_STR) || match(TokenType::TYPE_BOOL) ||
                   match(TokenType::TYPE_VAR)) {
            return parseTypeStatement();
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
        } else if (match(TokenType::HTTP)) {
            return parseHttp();
        } else if (match(TokenType::KEYPRESS)) {
            return parseKeypress();
        } else if (match(TokenType::HOTKEY)) {
            return parseHotkey();
        } else if (match(TokenType::AI)) {
            return parseAI();
        } else if (match(TokenType::CLEAR)) {
            match(TokenType::LPAREN);
            match(TokenType::RPAREN);
            match(TokenType::SEMICOLON);
            return std::make_shared<GraphicsStmt>("clear");
        } else if (match(TokenType::DRAW)) {
            return parseDraw();
        } else if (match(TokenType::RENDER)) {
            match(TokenType::LPAREN);
            match(TokenType::RPAREN);
            match(TokenType::SEMICOLON);
            return std::make_shared<GraphicsStmt>("render");
        } else if (match(TokenType::WINDOW)) {
            return parseWindow();
        } else if (match(TokenType::GL_INIT)) {
            match(TokenType::LPAREN);
            match(TokenType::RPAREN);
            match(TokenType::SEMICOLON);
            return std::make_shared<GraphicsStmt>("gl_init");
        } else if (match(TokenType::SET_COLOR)) {
            return parseGraphicsFunction("set_color");
        } else if (match(TokenType::SET_CLEAR_COLOR)) {
            return parseGraphicsFunction("set_clear_color");
        } else if (match(TokenType::DRAW_TRIANGLE)) {
            return parseGraphicsFunction("draw_triangle");
        } else if (match(TokenType::DRAW_QUAD)) {
            return parseGraphicsFunction("draw_quad");
        } else if (match(TokenType::FILL_RECT)) {
            return parseGraphicsFunction("fill_rect");
        } else if (match(TokenType::DRAW_CIRCLE)) {
            return parseGraphicsFunction("draw_circle");
        } else if (match(TokenType::FILL_CIRCLE)) {
            return parseGraphicsFunction("fill_circle");
        } else if (match(TokenType::SET_PROJECTION_2D)) {
            match(TokenType::LPAREN);
            match(TokenType::RPAREN);
            match(TokenType::SEMICOLON);
            return std::make_shared<GraphicsStmt>("set_projection_2d");
        } else if (match(TokenType::SET_PROJECTION_3D)) {
            return parseGraphicsFunction("set_projection_3d");
        } else if (match(TokenType::TRANSLATE)) {
            return parseGraphicsFunction("translate");
        } else if (match(TokenType::ROTATE)) {
            return parseGraphicsFunction("rotate");
        } else if (match(TokenType::SCALE)) {
            return parseGraphicsFunction("scale");
        } else if (match(TokenType::PUSH_MATRIX)) {
            match(TokenType::LPAREN);
            match(TokenType::RPAREN);
            match(TokenType::SEMICOLON);
            return std::make_shared<GraphicsStmt>("push_matrix");
        } else if (match(TokenType::POP_MATRIX)) {
            match(TokenType::LPAREN);
            match(TokenType::RPAREN);
            match(TokenType::SEMICOLON);
            return std::make_shared<GraphicsStmt>("pop_matrix");
        } else if (match(TokenType::WINDOW_SHOULD_CLOSE)) {
            match(TokenType::LPAREN);
            match(TokenType::RPAREN);
            match(TokenType::SEMICOLON);
            return std::make_shared<GraphicsStmt>("window_should_close");
        } else if (match(TokenType::INCLUDE)) {
            return parseInclude();
        } else if (check(TokenType::IDENT)) {
            return parseAssignmentOrExpr();
        }
        
        if (check(TokenType::SEMICOLON)) {
            advance();
            return nullptr;
        }
        
        // Unknown token
        if (!check(TokenType::END_OF_FILE) && !check(TokenType::RBRACE)) {
            addError("Unexpected token in statement");
            advance();
        }
        return nullptr;
    }

    StmtPtr parseHttp() {
        auto url = parseExpression();
        auto method = parseExpression();
        std::string methodStr = "GET";
        if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(method)) {
            methodStr = strLit->value;
        }
        std::string resultVar = "";
        if (check(TokenType::IDENT)) {
            resultVar = peek().lexeme;
            advance();
        }
        match(TokenType::SEMICOLON);
        return std::make_shared<HttpStmt>(url, methodStr, resultVar);
    }

    StmtPtr parseKeypress() {
        match(TokenType::LPAREN);
        match(TokenType::RPAREN);
        std::string resultVar = "";
        if (check(TokenType::IDENT)) {
            resultVar = peek().lexeme;
            advance();
        }
        match(TokenType::SEMICOLON);
        return std::make_shared<KeypressStmt>(resultVar);
    }

    StmtPtr parseHotkey() {
        expect(TokenType::LPAREN, "Expected '(' after hotkey");
        auto key = parseExpression();
        expect(TokenType::COMMA, "Expected ','");
        std::string callback = peek().lexeme;
        expect(TokenType::IDENT, "Expected callback name");
        expect(TokenType::RPAREN, "Expected ')'");
        match(TokenType::SEMICOLON);
        return std::make_shared<HotkeyStmt>(key, callback);
    }

    StmtPtr parseAI() {
        expect(TokenType::LPAREN, "Expected '(' after ai_query");
        auto prompt = parseExpression();
        expect(TokenType::RPAREN, "Expected ')'");
        std::string resultVar = "";
        if (check(TokenType::IDENT)) {
            resultVar = peek().lexeme;
            advance();
        }
        match(TokenType::SEMICOLON);
        return std::make_shared<AIStmt>(prompt, resultVar);
    }

    StmtPtr parseDraw() {
        std::string cmd = previous().lexeme;
        std::vector<ExprPtr> args;
        expect(TokenType::LPAREN, "Expected '('");
        while (!check(TokenType::RPAREN) && !check(TokenType::END_OF_FILE)) {
            args.push_back(parseExpression());
            if (!check(TokenType::RPAREN)) {
                if (!match(TokenType::COMMA)) break;
            }
        }
        expect(TokenType::RPAREN, "Expected ')'");
        match(TokenType::SEMICOLON);
        return std::make_shared<GraphicsStmt>(cmd, args);
    }

    StmtPtr parseWindow() {
        std::vector<ExprPtr> args;
        expect(TokenType::LPAREN, "Expected '('");
        while (!check(TokenType::RPAREN) && !check(TokenType::END_OF_FILE)) {
            args.push_back(parseExpression());
            if (!check(TokenType::RPAREN)) {
                if (!match(TokenType::COMMA)) break;
            }
        }
        expect(TokenType::RPAREN, "Expected ')'");
        match(TokenType::SEMICOLON);
        return std::make_shared<GraphicsStmt>("window_create", args);
    }

    StmtPtr parseInclude() {
        std::string path = peek().lexeme;
        expect(TokenType::STRING, "Expected string path after include");
        match(TokenType::SEMICOLON);
        return std::make_shared<IncludeStmt>(path);
    }

    StmtPtr parseGraphicsFunction(const std::string& cmd) {
        std::vector<ExprPtr> args;
        expect(TokenType::LPAREN, "Expected '('");
        while (!check(TokenType::RPAREN) && !check(TokenType::END_OF_FILE)) {
            args.push_back(parseExpression());
            if (!check(TokenType::RPAREN)) {
                if (!match(TokenType::COMMA)) break;
            }
        }
        expect(TokenType::RPAREN, "Expected ')'");
        match(TokenType::SEMICOLON);
        return std::make_shared<GraphicsStmt>(cmd, args);
    }

    StmtPtr parsePrint() {
        // Stream syntax: print("text" << x << "more\n")
        // Simple syntax: print expr
        if (match(TokenType::LPAREN)) {
            // Stream syntax
            std::vector<ExprPtr> parts;
            
            // Parse first part
            auto first = parseExpression();
            parts.push_back(first);
            
            // Parse << chain
            while (match(TokenType::SHL)) {
                auto next = parseExpression();
                parts.push_back(next);
            }
            
            expect(TokenType::RPAREN, "Expected ')' after print arguments");
            match(TokenType::SEMICOLON);
            
            if (parts.size() == 1) {
                return std::make_shared<PrintStmt>(parts[0], true);
            }
            return std::make_shared<PrintStmt>(std::make_shared<StreamExpr>(parts), true);
        }
        
        // Simple syntax: print expr;
        auto expr = parseExpression();
        match(TokenType::SEMICOLON);
        return std::make_shared<PrintStmt>(expr, false);
    }

    StmtPtr parseInput() {
        // Python-style: input(var, "prompt")
        // Also supports: input<type>("prompt", var) or input "prompt" var;
        std::string varType = "str";  // default type
        std::string prompt = "";
        std::string varName = "";
        
        // Check for Python-style: input(var, "prompt")
        if (match(TokenType::LPAREN)) {
            // First arg: variable name (possibly with type prefix like float << var)
            if (check(TokenType::TYPE_INT) || check(TokenType::TYPE_FLOAT) || 
                check(TokenType::TYPE_STR) || check(TokenType::TYPE_BOOL)) {
                // Type specified: input(int << var, "prompt")
                if (check(TokenType::TYPE_INT)) varType = "int";
                else if (check(TokenType::TYPE_FLOAT)) varType = "float";
                else if (check(TokenType::TYPE_STR)) varType = "str";
                else if (check(TokenType::TYPE_BOOL)) varType = "bool";
                advance();
                if (match(TokenType::SHL)) {
                    // type << var
                    if (check(TokenType::IDENT)) {
                        varName = peek().lexeme;
                        advance();
                    }
                }
            } else if (check(TokenType::IDENT)) {
                varName = peek().lexeme;
                advance();
            }
            
            // Optional comma and prompt
            if (match(TokenType::COMMA)) {
                if (check(TokenType::STRING)) {
                    prompt = peek().lexeme;
                    advance();
                }
            }
            expect(TokenType::RPAREN, "Expected ')' after input arguments");
            match(TokenType::SEMICOLON);
            return std::make_shared<InputStmt>(prompt, varName, varType);
        }
        
        // Legacy syntax: input<type> "prompt" varName;
        if (match(TokenType::LT)) {
            if (check(TokenType::TYPE_INT)) {
                varType = "int";
                advance();
            } else if (check(TokenType::TYPE_STR)) {
                varType = "str";
                advance();
            } else if (check(TokenType::TYPE_FLOAT)) {
                varType = "float";
                advance();
            } else if (check(TokenType::TYPE_BOOL)) {
                varType = "bool";
                advance();
            } else if (check(TokenType::IDENT)) {
                std::string typeName = peek().lexeme;
                if (typeName == "int") varType = "int";
                else if (typeName == "str" || typeName == "string") varType = "str";
                else if (typeName == "float") varType = "float";
                else if (typeName == "bool") varType = "bool";
                advance();
            }
            expect(TokenType::GT, "Expected '>' after type in input<type>");
        }
        
        if (check(TokenType::STRING)) {
            prompt = peek().lexeme;
            advance();
        }
        if (check(TokenType::IDENT)) {
            varName = peek().lexeme;
            advance();
        }
        match(TokenType::SEMICOLON);
        return std::make_shared<InputStmt>(prompt, varName, varType);
    }

    // Parse type-prefixed statements: int x; float << x; var c;
    StmtPtr parseTypeStatement() {
        Token typeToken = previous();
        std::string typeName;
        if (typeToken.type == TokenType::TYPE_INT) typeName = "int";
        else if (typeToken.type == TokenType::TYPE_FLOAT) typeName = "float";
        else if (typeToken.type == TokenType::TYPE_STR) typeName = "str";
        else if (typeToken.type == TokenType::TYPE_BOOL) typeName = "bool";
        else if (typeToken.type == TokenType::TYPE_VAR) typeName = "var";
        
        // Check for conversion: type << var;
        if (match(TokenType::SHL)) {
            if (check(TokenType::IDENT)) {
                std::string varName = peek().lexeme;
                advance();
                match(TokenType::SEMICOLON);
                return std::make_shared<TypeConvert>(typeName, varName);
            }
        }
        
        // Variable declaration: int x; or int x = value; or var c = "...";
        if (check(TokenType::IDENT)) {
            std::string varName = peek().lexeme;
            advance();
            
            ExprPtr initValue = nullptr;
            if (match(TokenType::EQ)) {
                initValue = parseExpression();
            }
            match(TokenType::SEMICOLON);
            return std::make_shared<VarDecl>(typeName, varName, initValue);
        }
        
        addError("Expected variable name after type");
        return nullptr;
    }

    StmtPtr parseIf() {
        expect(TokenType::LPAREN, "Expected '(' after 'if'");
        auto cond = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after condition");
        auto thenBlock = parseBlock();
        std::vector<StmtPtr> elseBlock;
        if (match(TokenType::ELSE)) {
            if (check(TokenType::IF)) {
                // else if
                advance();
                auto elseIf = parseIf();
                elseBlock.push_back(elseIf);
            } else {
                elseBlock = parseBlock();
            }
        }
        return std::make_shared<IfStmt>(cond, thenBlock, elseBlock);
    }

    StmtPtr parseWhile() {
        expect(TokenType::LPAREN, "Expected '(' after 'while'");
        auto cond = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after condition");
        auto body = parseBlock();
        return std::make_shared<WhileStmt>(cond, body);
    }

    StmtPtr parseFor() {
        expect(TokenType::LPAREN, "Expected '(' after 'for'");
        std::string var = peek().lexeme;
        expect(TokenType::IDENT, "Expected loop variable name");
        expect(TokenType::EQ, "Expected '=' in for loop");
        auto start = parseExpression();
        
        // Support both "for (i = 0; end)" and "for (i = 0 to end)" syntax
        if (check(TokenType::IDENT) && peek().lexeme == "to") {
            advance();  // skip "to"
        } else {
            expect(TokenType::SEMICOLON, "Expected ';' or 'to' after start value");
        }
        
        auto end = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after for clause");
        auto body = parseBlock();
        return std::make_shared<ForStmt>(var, start, end, body);
    }

    StmtPtr parseFunction() {
        std::string name = peek().lexeme;
        expect(TokenType::IDENT, "Expected function name");
        
        std::vector<std::pair<std::string, std::string>> params;
        expect(TokenType::LPAREN, "Expected '(' after function name");
        while (!check(TokenType::RPAREN) && !check(TokenType::END_OF_FILE)) {
            std::string pname, ptype = "int";
            
            // Support both "type name" and "name: type" syntax
            if (check(TokenType::TYPE_INT) || check(TokenType::TYPE_FLOAT) || 
                check(TokenType::TYPE_STR) || check(TokenType::TYPE_BOOL) ||
                check(TokenType::TYPE_VAR)) {
                // C-style: type name
                if (check(TokenType::TYPE_INT)) ptype = "int";
                else if (check(TokenType::TYPE_FLOAT)) ptype = "float";
                else if (check(TokenType::TYPE_STR)) ptype = "str";
                else if (check(TokenType::TYPE_BOOL)) ptype = "bool";
                else if (check(TokenType::TYPE_VAR)) ptype = "var";
                advance();
                pname = peek().lexeme;
                expect(TokenType::IDENT, "Expected parameter name");
            } else if (check(TokenType::IDENT)) {
                // Could be "name: type" or just "name" (default int)
                pname = peek().lexeme;
                advance();
                if (match(TokenType::COLON)) {
                    // name: type syntax
                    if (check(TokenType::TYPE_INT)) ptype = "int";
                    else if (check(TokenType::TYPE_FLOAT)) ptype = "float";
                    else if (check(TokenType::TYPE_STR)) ptype = "str";
                    else if (check(TokenType::TYPE_BOOL)) ptype = "bool";
                    else if (check(TokenType::IDENT)) ptype = peek().lexeme;
                    advance();
                }
            }
            
            if (!pname.empty()) {
                params.push_back({pname, ptype});
            }
            if (!check(TokenType::RPAREN)) {
                if (!match(TokenType::COMMA)) break;
            }
        }
        expect(TokenType::RPAREN, "Expected ')' after parameters");
        
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
        while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENT)) {
                std::string fname = peek().lexeme;
                advance();
                std::string ftype = "int";
                if (match(TokenType::COLON)) {
                    ftype = peek().lexeme;
                    expect(TokenType::IDENT, "Expected field type");
                }
                cls->fields.push_back({fname, ftype});
                match(TokenType::SEMICOLON);
            } else if (match(TokenType::FUNC)) {
                // Method - skip for now
                parseFunction();
            } else {
                advance();
            }
        }
        expect(TokenType::RBRACE, "Expected '}' after class body");
        return cls;
    }

    StmtPtr parseReturn() {
        ExprPtr value = nullptr;
        if (!check(TokenType::SEMICOLON) && !check(TokenType::RBRACE)) {
            value = parseExpression();
        }
        match(TokenType::SEMICOLON);
        return std::make_shared<ReturnStmt>(value);
    }

    StmtPtr parseAssignmentOrExpr() {
        std::string name = peek().lexeme;
        advance();
        
        // Format output: x >> 0x; x >> bin; etc.
        if (match(TokenType::SHR)) {
            std::string format = "hex";  // default
            if (check(TokenType::FMT_BIN) || (check(TokenType::IDENT) && peek().lexeme == "0b")) {
                format = "bin";
                advance();
            } else if (check(TokenType::FMT_HEX) || (check(TokenType::IDENT) && peek().lexeme == "0x")) {
                format = "hex";
                advance();
            } else if (check(TokenType::FMT_OCT) || (check(TokenType::IDENT) && peek().lexeme == "0o")) {
                format = "oct";
                advance();
            } else if (check(TokenType::FMT_DEC) || (check(TokenType::IDENT) && peek().lexeme == "0d")) {
                format = "dec";
                advance();
            } else if (check(TokenType::IDENT)) {
                // Allow bin, hex, oct, dec as identifiers too
                std::string fmt = peek().lexeme;
                if (fmt == "bin" || fmt == "binary") format = "bin";
                else if (fmt == "hex") format = "hex";
                else if (fmt == "oct" || fmt == "octal") format = "oct";
                else if (fmt == "dec" || fmt == "decimal") format = "dec";
                advance();
            }
            match(TokenType::SEMICOLON);
            return std::make_shared<FormatOutput>(name, format);
        }
        
        // Array declaration: name[] = "string" or name[] = [1, 2, 3]
        if (match(TokenType::LBRACKET)) {
            if (match(TokenType::RBRACKET)) {
                // name[] = value (array from string or array literal)
                if (match(TokenType::EQ)) {
                    auto value = parseExpression();
                    match(TokenType::SEMICOLON);
                    // Convert to array declaration
                    std::vector<ExprPtr> elements;
                    if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(value)) {
                        // Convert string to char array: "abc" -> ['a', 'b', 'c']
                        for (char c : strLit->value) {
                            elements.push_back(std::make_shared<StringLiteral>(std::string(1, c)));
                        }
                    } else if (auto arrLit = std::dynamic_pointer_cast<ArrayLiteral>(value)) {
                        elements = arrLit->elements;
                    } else {
                        elements.push_back(value);
                    }
                    return std::make_shared<Assignment>(name, std::make_shared<ArrayLiteral>(elements));
                }
            } else {
                // name[index] = value (array element assignment)
                auto index = parseExpression();
                expect(TokenType::RBRACKET, "Expected ']'");
                if (match(TokenType::EQ)) {
                    auto value = parseExpression();
                    match(TokenType::SEMICOLON);
                    return std::make_shared<ArrayAssignment>(name, index, value);
                }
            }
        } else if (match(TokenType::EQ)) {
            // Check for format conversion: decv = c >> 0x;
            if (check(TokenType::IDENT)) {
                std::string srcVar = peek().lexeme;
                advance();
                if (match(TokenType::SHR)) {
                    std::string format = "hex";  // default
                    if (check(TokenType::FMT_BIN) || (check(TokenType::IDENT) && (peek().lexeme == "0b" || peek().lexeme == "bin"))) {
                        format = "bin";
                        advance();
                    } else if (check(TokenType::FMT_HEX) || (check(TokenType::IDENT) && (peek().lexeme == "0x" || peek().lexeme == "hex"))) {
                        format = "hex";
                        advance();
                    } else if (check(TokenType::FMT_OCT) || (check(TokenType::IDENT) && (peek().lexeme == "0o" || peek().lexeme == "oct"))) {
                        format = "oct";
                        advance();
                    } else if (check(TokenType::FMT_DEC) || (check(TokenType::IDENT) && (peek().lexeme == "0d" || peek().lexeme == "dec"))) {
                        format = "dec";
                        advance();
                    }
                    match(TokenType::SEMICOLON);
                    return std::make_shared<Assignment>(name, std::make_shared<FormatConvert>(srcVar, format));
                }
                // Not a format conversion, backtrack and parse as normal expression
                current--;  // Go back to re-parse the identifier
            }
            auto value = parseExpression();
            match(TokenType::SEMICOLON);
            return std::make_shared<Assignment>(name, value);
        } else if (match(TokenType::LPAREN)) {
            // Function call as statement
            std::vector<ExprPtr> args;
            while (!check(TokenType::RPAREN) && !check(TokenType::END_OF_FILE)) {
                args.push_back(parseExpression());
                if (!check(TokenType::RPAREN)) {
                    if (!match(TokenType::COMMA)) break;
                }
            }
            expect(TokenType::RPAREN, "Expected ')'");
            match(TokenType::SEMICOLON);
            auto call = std::make_shared<FunctionCall>(name, args);
            return std::make_shared<Assignment>("_", call);
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
            return std::make_shared<IntLiteral>(std::stol(previous().lexeme));
        }
        if (match(TokenType::FLOAT)) {
            return std::make_shared<FloatLiteral>(std::stod(previous().lexeme));
        }
        if (match(TokenType::STRING)) {
            return std::make_shared<StringLiteral>(previous().lexeme);
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
                while (!check(TokenType::RPAREN) && !check(TokenType::END_OF_FILE)) {
                    args.push_back(parseExpression());
                    if (!check(TokenType::RPAREN)) {
                        if (!match(TokenType::COMMA)) break;
                    }
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
            while (!check(TokenType::RBRACKET) && !check(TokenType::END_OF_FILE)) {
                elements.push_back(parseExpression());
                if (!check(TokenType::RBRACKET)) {
                    if (!match(TokenType::COMMA)) break;
                }
            }
            expect(TokenType::RBRACKET, "Expected ']'");
            return std::make_shared<ArrayLiteral>(elements);
        }
        
        addError("Expected expression");
        return std::make_shared<IntLiteral>(0); // Return dummy value
    }
};

} // namespace disp

#endif // DISPLEXITY_PARSER_H
