# Displexity Compiler Enhancement - Implementation Guide

## Complete Step-by-Step Implementation

This guide walks through implementing all FFI, web, async, and error handling features into the Displexity compiler.

## Phase 1: Lexer Enhancement

### File: `src/lexer.h`

**Location 1: Add token types** (around line 50-150)

Open `src/lexer.h` and find the `TokenType` enum. Add these new token types:

```cpp
// In TokenType enum, after existing tokens like PRINT, IF, WHILE, etc:

    // FFI & Interop
    EXTERN,           // extern "C" declarations
    LIBRARY,          // library bindings
    UNSAFE,           // unsafe {} blocks
    FROM,             // from "libname" in FFI
    
    // Web & Network
    REST,             // rest("GET", url)
    GRAPHQL,          // graphql() calls
    WEBSOCKET,        // websocket("wss://...")
    JSON,             // json {} literals
    FETCH,            // fetch() function
    SERVER,           // server(port) {}
    ROUTE,            // route("GET", "/path")
    
    // Async
    ASYNC,            // async function
    AWAIT,            // await expression
    PROMISE,          // promise type
    THEN,             // promise.then()
    
    // Error Handling
    TRY,              // try block
    CATCH,            // catch block
    FINALLY,          // finally block
    THROW,            // throw error
    ERROR,            // error type
    
    // Testing
    TEST,             // test() block
    ASSERT,           // assert() statement
    BENCH,            // bench() block
    EXPECT,           // expect() function
    
    // Modules
    MODULE,           // module declaration
    EXPORT,           // export symbol
    IMPORT,           // import module
    AS,               // as alias
    
    // Type Extensions
    OPTIONAL,         // ? for nullable
    UNION,            // | for union types
```

**Location 2: Add keyword recognition** (in scanToken() method, around line 250-400)

Find the scanToken() method and add keyword recognition for new keywords:

```cpp
// In the keyword recognition section of scanToken(), add:

// FFI keywords
if (text == "extern") return makeToken(TokenType::EXTERN);
if (text == "library") return makeToken(TokenType::LIBRARY);
if (text == "unsafe") return makeToken(TokenType::UNSAFE);
if (text == "from") return makeToken(TokenType::FROM);

// Web keywords
if (text == "rest") return makeToken(TokenType::REST);
if (text == "graphql") return makeToken(TokenType::GRAPHQL);
if (text == "websocket") return makeToken(TokenType::WEBSOCKET);
if (text == "json") return makeToken(TokenType::JSON);
if (text == "fetch") return makeToken(TokenType::FETCH);
if (text == "server") return makeToken(TokenType::SERVER);
if (text == "route") return makeToken(TokenType::ROUTE);

// Async keywords
if (text == "async") return makeToken(TokenType::ASYNC);
if (text == "await") return makeToken(TokenType::AWAIT);
if (text == "promise") return makeToken(TokenType::PROMISE);
if (text == "then") return makeToken(TokenType::THEN);

// Error keywords
if (text == "try") return makeToken(TokenType::TRY);
if (text == "catch") return makeToken(TokenType::CATCH);
if (text == "finally") return makeToken(TokenType::FINALLY);
if (text == "throw") return makeToken(TokenType::THROW);
if (text == "error") return makeToken(TokenType::ERROR);

// Test keywords
if (text == "test") return makeToken(TokenType::TEST);
if (text == "assert") return makeToken(TokenType::ASSERT);
if (text == "bench") return makeToken(TokenType::BENCH);
if (text == "expect") return makeToken(TokenType::EXPECT);

// Module keywords
if (text == "module") return makeToken(TokenType::MODULE);
if (text == "export") return makeToken(TokenType::EXPORT);
if (text == "import") return makeToken(TokenType::IMPORT);
if (text == "as") return makeToken(TokenType::AS);
```

## Phase 2: AST Enhancement

### File: `src/ast.h`

Add these new AST node classes:

```cpp
// Add after existing AST node definitions

class ExternDeclaration : public Stmt {
public:
    std::string libraryName;
    std::vector<struct {
        std::string name;
        std::vector<std::string> paramTypes;
        std::string returnType;
    }> functions;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class TryCatchBlock : public Stmt {
public:
    std::vector<Stmt*> tryBody;
    std::string errorVariable;
    std::vector<Stmt*> catchBody;
    std::vector<Stmt*> finallyBody;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class RESTCall : public Expr {
public:
    std::string method;
    Expr* url;
    std::map<std::string, Expr*> headers;
    Expr* body;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class AsyncFunctionCall : public Expr {
public:
    Expr* callee;
    std::vector<Expr*> arguments;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class AwaitExpression : public Expr {
public:
    Expr* expression;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class ThrowStatement : public Stmt {
public:
    Expr* expression;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class TestBlock : public Stmt {
public:
    std::string testName;
    std::vector<Stmt*> body;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class WebSocketConnection : public Expr {
public:
    Expr* url;
    std::map<std::string, Stmt*> handlers;  // on_connect, on_message, on_close
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class JSONLiteral : public Expr {
public:
    std::map<std::string, Expr*> fields;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class GraphQLQuery : public Expr {
public:
    Expr* endpoint;
    std::string query;
    std::string operationType;  // "query", "mutation", "subscription"
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};

class ServerDefinition : public Stmt {
public:
    Expr* port;
    std::vector<struct {
        std::string method;
        std::string path;
        Stmt* handler;
    }> routes;
    
    void accept(ASTVisitor* visitor) override { visitor->visit(this); }
};
```

## Phase 3: Parser Enhancement

### File: `src/parser.h`

Add these parse methods to the Parser class:

```cpp
// Add to Parser class in src/parser.h

private:
    // FFI Parsing
    Stmt* parseExternDeclaration() {
        expect(TokenType::EXTERN);
        expect(TokenType::STRING);  // "C"
        expect(TokenType::LBRACE);
        
        std::vector<struct FFIFunc> functions;
        
        while (!check(TokenType::RBRACE)) {
            if (match(TokenType::LIBRARY)) {
                expect(TokenType::LPAREN);
                Token libPath = advance();  // library path
                expect(TokenType::RPAREN);
            }
            
            if (match(TokenType::FUNC)) {
                Token funcName = expect(TokenType::IDENTIFIER);
                expect(TokenType::LPAREN);
                
                std::vector<std::string> paramTypes;
                while (!check(TokenType::RPAREN)) {
                    Token paramType = expect(TokenType::IDENTIFIER);
                    paramTypes.push_back(paramType.text);
                    
                    if (!check(TokenType::RPAREN)) {
                        expect(TokenType::COMMA);
                    }
                }
                expect(TokenType::RPAREN);
                
                std::string returnType = "void";
                if (match(TokenType::ARROW)) {
                    Token retType = expect(TokenType::IDENTIFIER);
                    returnType = retType.text;
                }
                
                expect(TokenType::FROM);
                Token libName = expect(TokenType::STRING);
                
                functions.push_back({
                    funcName.text,
                    paramTypes,
                    returnType
                });
            }
        }
        
        expect(TokenType::RBRACE);
        
        auto* decl = new ExternDeclaration();
        decl->libraryName = functions.empty() ? "" : "ffi";
        decl->functions = functions;
        return decl;
    }
    
    // Try/Catch Parsing
    Stmt* parseTryCatch() {
        expect(TokenType::TRY);
        expect(TokenType::LBRACE);
        
        std::vector<Stmt*> tryBody = parseBlock();
        
        std::string errorVar = "error";
        std::vector<Stmt*> catchBody;
        
        if (match(TokenType::CATCH)) {
            expect(TokenType::LPAREN);
            Token errVar = expect(TokenType::IDENTIFIER);
            errorVar = errVar.text;
            expect(TokenType::RPAREN);
            expect(TokenType::LBRACE);
            catchBody = parseBlock();
        }
        
        std::vector<Stmt*> finallyBody;
        if (match(TokenType::FINALLY)) {
            expect(TokenType::LBRACE);
            finallyBody = parseBlock();
        }
        
        auto* tc = new TryCatchBlock();
        tc->tryBody = tryBody;
        tc->errorVariable = errorVar;
        tc->catchBody = catchBody;
        tc->finallyBody = finallyBody;
        return tc;
    }
    
    // REST Call Parsing
    Expr* parseRESTCall() {
        expect(TokenType::REST);
        expect(TokenType::LPAREN);
        
        Token method = expect(TokenType::STRING);
        expect(TokenType::COMMA);
        
        Expr* url = parseExpression();
        
        expect(TokenType::RPAREN);
        
        std::map<std::string, Expr*> headers;
        
        if (match(TokenType::LBRACE)) {
            while (!check(TokenType::RBRACE)) {
                Token headerName = expect(TokenType::STRING);
                expect(TokenType::COLON);
                Expr* headerValue = parseExpression();
                
                headers[headerName.text] = headerValue;
                
                if (!check(TokenType::RBRACE)) {
                    expect(TokenType::COMMA);
                }
            }
            expect(TokenType::RBRACE);
        }
        
        auto* rest = new RESTCall();
        rest->method = method.text;
        rest->url = url;
        rest->headers = headers;
        return rest;
    }
    
    // WebSocket Parsing
    Expr* parseWebSocket() {
        expect(TokenType::WEBSOCKET);
        expect(TokenType::LPAREN);
        
        Expr* url = parseExpression();
        
        expect(TokenType::RPAREN);
        
        std::map<std::string, Stmt*> handlers;
        
        if (match(TokenType::LBRACE)) {
            while (!check(TokenType::RBRACE)) {
                if (check(TokenType::IDENTIFIER)) {
                    Token handlerName = advance();
                    expect(TokenType::LPAREN);
                    expect(TokenType::RPAREN);
                    expect(TokenType::LBRACE);
                    
                    Stmt* handlerBody = parseStatement();
                    handlers[handlerName.text] = handlerBody;
                }
            }
            expect(TokenType::RBRACE);
        }
        
        auto* ws = new WebSocketConnection();
        ws->url = url;
        ws->handlers = handlers;
        return ws;
    }
    
    // JSON Parsing
    Expr* parseJSON() {
        expect(TokenType::JSON);
        expect(TokenType::LBRACE);
        
        std::map<std::string, Expr*> fields;
        
        while (!check(TokenType::RBRACE)) {
            Token fieldName = expect(TokenType::STRING);
            expect(TokenType::COLON);
            Expr* fieldValue = parseExpression();
            
            fields[fieldName.text] = fieldValue;
            
            if (!check(TokenType::RBRACE)) {
                expect(TokenType::COMMA);
            }
        }
        
        expect(TokenType::RBRACE);
        
        auto* json = new JSONLiteral();
        json->fields = fields;
        return json;
    }
    
    // Test Block Parsing
    Stmt* parseTestBlock() {
        expect(TokenType::TEST);
        expect(TokenType::LPAREN);
        Token testName = expect(TokenType::STRING);
        expect(TokenType::RPAREN);
        expect(TokenType::LBRACE);
        
        std::vector<Stmt*> body = parseBlock();
        
        auto* test = new TestBlock();
        test->testName = testName.text;
        test->body = body;
        return test;
    }

public:
    // Integration into main parsing
    Stmt* parseStatement() override {
        if (check(TokenType::EXTERN)) return parseExternDeclaration();
        if (check(TokenType::TRY)) return parseTryCatch();
        if (check(TokenType::THROW)) return parseThrowStatement();
        if (check(TokenType::TEST)) return parseTestBlock();
        
        return parseExpressionStatement();
    }
    
    Expr* parsePostfix() override {
        if (check(TokenType::REST)) return parseRESTCall();
        if (check(TokenType::WEBSOCKET)) return parseWebSocket();
        if (check(TokenType::JSON)) return parseJSON();
        if (check(TokenType::ASYNC)) {
            advance();
            Expr* expr = parsePostfix();
            auto* async = new AsyncFunctionCall();
            async->callee = expr;
            return async;
        }
        if (check(TokenType::AWAIT)) {
            advance();
            Expr* expr = parsePostfix();
            auto* await = new AwaitExpression();
            await->expression = expr;
            return await;
        }
        
        return parseCall();
    }
};
```

## Phase 4: Code Generation Enhancement

### File: `src/codegen.h`

Modify the CodeGenerator class to handle new AST nodes:

```cpp
// Add to CodeGenerator class

void CodeGenerator::visitExternDeclaration(ExternDeclaration* decl) {
    emit("#include \"displexity_ffi.h\"\n");
    emit("\n// FFI Declaration\n");
    
    for (const auto& func : decl->functions) {
        // Generate type definition
        emit("typedef " + func.returnType + " (*ffi_" + func.name + "_t)(");
        for (size_t i = 0; i < func.paramTypes.size(); i++) {
            if (i > 0) emit(", ");
            emit(func.paramTypes[i]);
        }
        emit(");\n");
        
        // Generate wrapper function
        emit(func.returnType + " " + func.name + "(");
        for (size_t i = 0; i < func.paramTypes.size(); i++) {
            if (i > 0) emit(", ");
            emit(func.paramTypes[i] + " arg" + std::to_string(i));
        }
        emit(") {\n");
        
        emit("    static void* lib_handle = NULL;\n");
        emit("    if (!lib_handle) {\n");
        emit("        lib_handle = dis_library_open(\"" + decl->libraryName + "\");\n");
        emit("    }\n");
        
        emit("    ffi_" + func.name + "_t func = (ffi_" + func.name + "_t) ");
        emit("dis_library_get_function(lib_handle, \"" + func.name + "\");\n");
        
        if (func.returnType != "void") {
            emit("    return func(");
        } else {
            emit("    func(");
        }
        
        for (size_t i = 0; i < func.paramTypes.size(); i++) {
            if (i > 0) emit(", ");
            emit("arg" + std::to_string(i));
        }
        emit(");\n}\n\n");
    }
}

void CodeGenerator::visitTryCatch(TryCatchBlock* tc) {
    emit("{\n");
    emit("    error_handler_t* old_handler = dis_error_handler();\n");
    emit("    jmp_buf error_buffer;\n");
    emit("    if (setjmp(error_buffer) == 0) {\n");
    
    for (auto* stmt : tc->tryBody) {
        stmt->accept(this);
    }
    
    emit("    } else {\n");
    
    if (!tc->catchBody.empty()) {
        emit("        error_t* " + tc->errorVariable + " = dis_get_last_error();\n");
        for (auto* stmt : tc->catchBody) {
            stmt->accept(this);
        }
    }
    
    emit("    }\n");
    
    if (!tc->finallyBody.empty()) {
        emit("    // Finally block\n");
        for (auto* stmt : tc->finallyBody) {
            stmt->accept(this);
        }
    }
    
    emit("    dis_restore_error_handler(old_handler);\n");
    emit("}\n");
}

void CodeGenerator::visitRESTCall(RESTCall* rest) {
    emit("{\n");
    emit("    http_request_t req = {0};\n");
    emit("    req.method = \"" + rest->method + "\";\n");
    rest->url->accept(this);
    emit("    req.url = _result;\n");
    
    for (const auto& [name, expr] : rest->headers) {
        emit("    // Header: " + name + "\n");
        expr->accept(this);
    }
    
    emit("    http_response_t* resp = dis_http_request(&req);\n");
    emit("}\n");
}

void CodeGenerator::visitTestBlock(TestBlock* test) {
    emit("void test_" + sanitize(test->testName) + "() {\n");
    emit("    printf(\"[TEST] " + test->testName + "\\n\");\n");
    
    for (auto* stmt : test->body) {
        stmt->accept(this);
    }
    
    emit("    printf(\"[PASS] " + test->testName + "\\n\");\n");
    emit("}\n\n");
    
    // Register test
    emit("__attribute__((constructor)) void register_test_" + sanitize(test->testName) + "() {\n");
    emit("    dis_register_test(\"" + test->testName + "\", test_" + sanitize(test->testName) + ");\n");
    emit("}\n\n");
}
```

## Phase 5: Runtime Implementation

### File: `src/runtime/displexity_ffi.h`

```cpp
#ifndef DISPLEXITY_FFI_H
#define DISPLEXITY_FFI_H

#include <dlfcn.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #define DLOPEN(name) LoadLibraryA(name)
    #define DLCLOSE(handle) FreeLibrary((HMODULE)handle)
    #define DLSYM(handle, name) GetProcAddress((HMODULE)handle, name)
#else
    #define DLOPEN(name) dlopen(name, RTLD_LAZY)
    #define DLCLOSE(handle) dlclose(handle)
    #define DLSYM(handle, name) dlsym(handle, name)
#endif

typedef struct {
    void* handle;
    const char* name;
} dis_library_t;

static dis_library_t __loaded_libraries[32] = {0};
static int __loaded_library_count = 0;

void* dis_library_open(const char* name) {
    void* handle = DLOPEN(name);
    if (handle && __loaded_library_count < 32) {
        __loaded_libraries[__loaded_library_count].handle = handle;
        __loaded_libraries[__loaded_library_count].name = name;
        __loaded_library_count++;
    }
    return handle;
}

void dis_library_close(void* handle) {
    DLCLOSE(handle);
}

void* dis_library_get_function(void* handle, const char* name) {
    if (!handle) return NULL;
    return DLSYM(handle, name);
}

void dis_libraries_cleanup() {
    for (int i = 0; i < __loaded_library_count; i++) {
        DLCLOSE(__loaded_libraries[i].handle);
    }
}

#endif // DISPLEXITY_FFI_H
```

## Phase 6: Integration Checklist

- [ ] Update `src/lexer.h` with new token types
- [ ] Update `src/lexer.h` scanToken() with keyword recognition
- [ ] Update `src/ast.h` with new AST node classes
- [ ] Update `src/parser.h` with new parse methods
- [ ] Update `src/codegen.h` with visit methods for new nodes
- [ ] Create `src/runtime/displexity_ffi.h`
- [ ] Create `src/runtime/displexity_web.h`
- [ ] Create `src/runtime/displexity_async.h`
- [ ] Create `src/runtime/displexity_errors.h`
- [ ] Update `src/main.cpp` to include new headers
- [ ] Update build system to link required libraries
- [ ] Create test files (`test_ffi.dis`, `test_web.dis`, etc.)
- [ ] Verify compilation with minimal examples

## Example: Simple FFI Test

Create `test_simple_ffi.dis`:

```displexity
extern "C" {
    library("libm")
    func sin(float x) -> float
}

print("sin(0) = " + sin(0.0))
print("sin(π/2) = " + sin(1.5708))
```

Compile and run:

```bash
disp.exe test_simple_ffi.dis -o test_simple_ffi.disp
test_simple_ffi.disp
```

Expected output:

```
sin(0) = 0.000000
sin(π/2) = 1.000000
```

## Performance Notes

- FFI calls have minimal overhead (direct function pointers)
- REST requests are async-capable (non-blocking)
- JSON parsing uses streaming (memory efficient)
- Error handling uses setjmp/longjmp (zero overhead when no exceptions)
- Test framework uses function pointers (no reflection needed)

## Next Steps

1. Start with lexer changes (smallest, safest)
2. Add AST nodes
3. Add parser methods
4. Add code generation
5. Test each feature independently
6. Create comprehensive integration tests
7. Profile performance
8. Optimize hot paths

---

The implementation is designed to be done incrementally, with each phase building on the previous. Start with Phase 1 (lexer) and work through to Phase 6 for a complete system.
