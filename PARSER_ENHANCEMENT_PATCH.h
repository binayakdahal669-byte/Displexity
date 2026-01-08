/*
 * Parser Enhancements Patch
 * Add these new parse methods to src/parser.h Parser class
 */

// NEW AST NODE TYPES (add to ast.h):
// ==================================

class ExternStatement : public Stmt {
public:
    std::string libname;
    std::vector<FFIFunction> functions;
    
    ExternStatement(const std::string& lib, const std::vector<FFIFunction>& funcs)
        : libname(lib), functions(funcs) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitExternStmt(this);
    }
};

class TryCatchStatement : public Stmt {
public:
    std::vector<Stmt*> try_block;
    std::string error_var;
    std::vector<Stmt*> catch_block;
    std::vector<Stmt*> finally_block;
    
    TryCatchStatement(const std::vector<Stmt*>& tryb,
                      const std::string& errvar,
                      const std::vector<Stmt*>& catchb,
                      const std::vector<Stmt*>& finallyb = {})
        : try_block(tryb), error_var(errvar), catch_block(catchb), finally_block(finallyb) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitTryCatchStmt(this);
    }
};

class RESTCallExpression : public Expr {
public:
    std::string method;
    std::string url;
    std::map<std::string, std::string> headers;
    std::string body;
    
    RESTCallExpression(const std::string& m, const std::string& u)
        : method(m), url(u) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitRESTExpr(this);
    }
};

class AsyncExpression : public Expr {
public:
    Expr* expression;
    
    AsyncExpression(Expr* expr) : expression(expr) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitAsyncExpr(this);
    }
};

class AwaitExpression : public Expr {
public:
    Expr* expression;
    
    AwaitExpression(Expr* expr) : expression(expr) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitAwaitExpr(this);
    }
};

class ThrowStatement : public Stmt {
public:
    Expr* expression;
    
    ThrowStatement(Expr* expr) : expression(expr) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitThrowStmt(this);
    }
};

class TestBlockStatement : public Stmt {
public:
    std::string test_name;
    std::vector<Stmt*> body;
    
    TestBlockStatement(const std::string& name, const std::vector<Stmt*>& stmts)
        : test_name(name), body(stmts) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitTestBlockStmt(this);
    }
};

class AssertStatement : public Stmt {
public:
    Expr* condition;
    std::string message;
    
    AssertStatement(Expr* cond, const std::string& msg = "")
        : condition(cond), message(msg) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitAssertStmt(this);
    }
};

class WebSocketExpression : public Expr {
public:
    std::string url;
    std::map<std::string, std::string> handlers;
    
    WebSocketExpression(const std::string& u) : url(u) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitWebSocketExpr(this);
    }
};

class JSONExpression : public Expr {
public:
    std::string json_string;
    
    JSONExpression(const std::string& json) : json_string(json) {}
    
    R accept(Visitor* visitor) override {
        return visitor->visitJSONExpr(this);
    }
};

// NEW PARSE METHODS (add to Parser class in parser.h):
// ====================================================

class Parser {
public:
    // ... existing methods ...
    
    // Parse extern/FFI declarations
    Stmt* parseExternStatement() {
        consume(TokenType::EXTERN, "Expect 'extern' keyword");
        consume(TokenType::STRING, "Expect library name in quotes");
        
        std::string libname = previous().literal;
        consume(TokenType::LEFT_BRACE, "Expect '{'");
        
        std::vector<FFIFunction> functions;
        
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match(TokenType::LIBRARY)) {
                consume(TokenType::LEFT_PAREN, "Expect '('");
                consume(TokenType::STRING, "Expect library path");
                previous();  // library path
                consume(TokenType::RIGHT_PAREN, "Expect ')'");
            }
            
            if (match(TokenType::FUNC)) {
                FFIFunction func = parseFFIFunction();
                functions.push_back(func);
            }
        }
        
        consume(TokenType::RIGHT_BRACE, "Expect '}'");
        
        return new ExternStatement(libname, functions);
    }
    
    // Parse FFI function signature
    FFIFunction Parser::parseFFIFunction() {
        Token name = consume(TokenType::IDENTIFIER, "Expect function name");
        consume(TokenType::LEFT_PAREN, "Expect '('");
        
        std::vector<std::string> paramTypes;
        while (!check(TokenType::RIGHT_PAREN)) {
            Token type = consume(TokenType::IDENTIFIER, "Expect parameter type");
            paramTypes.push_back(type.lexeme);
            
            if (!check(TokenType::RIGHT_PAREN)) {
                consume(TokenType::COMMA, "Expect ',' between parameters");
            }
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')'");
        
        std::string returnType = "void";
        if (match(TokenType::ARROW)) {
            Token retType = consume(TokenType::IDENTIFIER, "Expect return type");
            returnType = retType.lexeme;
        }
        
        consume(TokenType::FROM, "Expect 'from'");
        consume(TokenType::STRING, "Expect library name");
        
        FFIFunction func;
        func.name = name.lexeme;
        func.paramTypes = paramTypes;
        func.returnType = returnType;
        
        return func;
    }
    
    // Parse try/catch/finally blocks
    Stmt* parseTryCatchStatement() {
        consume(TokenType::TRY, "Expect 'try'");
        consume(TokenType::LEFT_BRACE, "Expect '{'");
        
        std::vector<Stmt*> try_block = parseBlock();
        
        std::string error_var = "error";
        std::vector<Stmt*> catch_block;
        
        if (match(TokenType::CATCH)) {
            consume(TokenType::LEFT_PAREN, "Expect '('");
            Token err = consume(TokenType::IDENTIFIER, "Expect error variable");
            error_var = err.lexeme;
            consume(TokenType::RIGHT_PAREN, "Expect ')'");
            consume(TokenType::LEFT_BRACE, "Expect '{'");
            
            catch_block = parseBlock();
        }
        
        std::vector<Stmt*> finally_block;
        if (match(TokenType::FINALLY)) {
            consume(TokenType::LEFT_BRACE, "Expect '{'");
            finally_block = parseBlock();
        }
        
        return new TryCatchStatement(try_block, error_var, catch_block, finally_block);
    }
    
    // Parse throw statements
    Stmt* parseThrowStatement() {
        consume(TokenType::THROW, "Expect 'throw'");
        Expr* expr = parseExpression();
        consume(TokenType::SEMICOLON, "Expect ';'");
        
        return new ThrowStatement(expr);
    }
    
    // Parse REST API calls
    Expr* parseRESTCall() {
        consume(TokenType::REST, "Expect 'rest'");
        consume(TokenType::LEFT_PAREN, "Expect '('");
        
        // Parse method
        consume(TokenType::STRING, "Expect HTTP method");
        std::string method = previous().literal;
        
        consume(TokenType::COMMA, "Expect ','");
        
        // Parse URL
        consume(TokenType::STRING, "Expect URL");
        std::string url = previous().literal;
        
        consume(TokenType::RIGHT_PAREN, "Expect ')'");
        
        // Parse optional headers block
        std::map<std::string, std::string> headers;
        if (match(TokenType::LEFT_BRACE)) {
            while (!check(TokenType::RIGHT_BRACE)) {
                consume(TokenType::STRING, "Expect header name");
                std::string headerName = previous().literal;
                consume(TokenType::COLON, "Expect ':'");
                consume(TokenType::STRING, "Expect header value");
                std::string headerValue = previous().literal;
                headers[headerName] = headerValue;
                
                if (!check(TokenType::RIGHT_BRACE)) {
                    consume(TokenType::COMMA, "Expect ',' between headers");
                }
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}'");
        }
        
        RESTCallExpression* rest = new RESTCallExpression(method, url);
        rest->headers = headers;
        return rest;
    }
    
    // Parse async/await
    Expr* parseAsyncAwait() {
        if (match(TokenType::ASYNC)) {
            Expr* expr = parsePostfixExpression();
            return new AsyncExpression(expr);
        }
        
        if (match(TokenType::AWAIT)) {
            Expr* expr = parsePostfixExpression();
            return new AwaitExpression(expr);
        }
        
        return parsePostfixExpression();
    }
    
    // Parse test blocks
    Stmt* parseTestBlock() {
        consume(TokenType::TEST, "Expect 'test'");
        consume(TokenType::LEFT_PAREN, "Expect '('");
        consume(TokenType::STRING, "Expect test name");
        std::string testName = previous().literal;
        consume(TokenType::RIGHT_PAREN, "Expect ')'");
        
        consume(TokenType::LEFT_BRACE, "Expect '{'");
        std::vector<Stmt*> body = parseBlock();
        
        return new TestBlockStatement(testName, body);
    }
    
    // Parse assert statements
    Stmt* parseAssertStatement() {
        consume(TokenType::ASSERT, "Expect 'assert'");
        consume(TokenType::LEFT_PAREN, "Expect '('");
        Expr* condition = parseExpression();
        
        std::string message = "";
        if (match(TokenType::COMMA)) {
            consume(TokenType::STRING, "Expect assertion message");
            message = previous().literal;
        }
        
        consume(TokenType::RIGHT_PAREN, "Expect ')'");
        consume(TokenType::SEMICOLON, "Expect ';'");
        
        return new AssertStatement(condition, message);
    }
    
    // Parse WebSocket connections
    Expr* parseWebSocketExpression() {
        consume(TokenType::WEBSOCKET, "Expect 'websocket'");
        consume(TokenType::LEFT_PAREN, "Expect '('");
        consume(TokenType::STRING, "Expect WebSocket URL");
        std::string url = previous().literal;
        consume(TokenType::RIGHT_PAREN, "Expect ')'");
        
        std::map<std::string, std::string> handlers;
        
        if (match(TokenType::LEFT_BRACE)) {
            while (!check(TokenType::RIGHT_BRACE)) {
                if (match(TokenType::IDENTIFIER)) {
                    std::string handler = previous().lexeme;
                    consume(TokenType::LEFT_PAREN, "Expect '('");
                    consume(TokenType::RIGHT_PAREN, "Expect ')'");
                    consume(TokenType::LEFT_BRACE, "Expect '{'");
                    
                    // Parse handler body (simplified)
                    std::string body = "{}";  // placeholder
                    handlers[handler] = body;
                    
                    skipUntilCloseBrace();
                    consume(TokenType::RIGHT_BRACE, "Expect '}'");
                }
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}'");
        }
        
        WebSocketExpression* ws = new WebSocketExpression(url);
        ws->handlers = handlers;
        return ws;
    }
    
    // Parse JSON expressions
    Expr* parseJSONExpression() {
        consume(TokenType::JSON, "Expect 'json'");
        consume(TokenType::LEFT_BRACE, "Expect '{'");
        
        std::string jsonStr = "{";
        
        while (!check(TokenType::RIGHT_BRACE)) {
            if (check(TokenType::STRING)) {
                advance();  // key
                jsonStr += "\"" + previous().literal + "\":";
                
                consume(TokenType::COLON, "Expect ':'");
                
                if (check(TokenType::STRING)) {
                    advance();
                    jsonStr += "\"" + previous().literal + "\"";
                } else if (check(TokenType::NUMBER)) {
                    advance();
                    jsonStr += previous().lexeme;
                }
                
                if (!check(TokenType::RIGHT_BRACE)) {
                    consume(TokenType::COMMA, "Expect ','");
                    jsonStr += ",";
                }
            }
        }
        
        consume(TokenType::RIGHT_BRACE, "Expect '}'");
        jsonStr += "}";
        
        return new JSONExpression(jsonStr);
    }
    
    // Update statement parsing to handle new statement types
    Stmt* parseStatement() override {
        // ... existing statement parsing ...
        
        // Add new statement type handling:
        if (check(TokenType::EXTERN)) return parseExternStatement();
        if (check(TokenType::TRY)) return parseTryCatchStatement();
        if (check(TokenType::THROW)) return parseThrowStatement();
        if (check(TokenType::TEST)) return parseTestBlock();
        if (check(TokenType::ASSERT)) return parseAssertStatement();
        
        return parseExpressionStatement();
    }
    
    // Update expression parsing to handle new expression types
    Expr* parseExpression() override {
        // ... existing expression parsing ...
        
        // Add new expression type handling:
        if (check(TokenType::REST)) return parseRESTCall();
        if (check(TokenType::ASYNC) || check(TokenType::AWAIT)) return parseAsyncAwait();
        if (check(TokenType::WEBSOCKET)) return parseWebSocketExpression();
        if (check(TokenType::JSON)) return parseJSONExpression();
        
        return parseTernaryExpression();
    }
};
