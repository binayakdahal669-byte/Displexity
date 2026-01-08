// Enhanced Displexity Lexer with FFI and Web Features
// Adds support for external function calls and REST/GraphQL
// File: src/lexer_enhanced.h

#ifndef DISPLEXITY_LEXER_ENHANCED_H
#define DISPLEXITY_LEXER_ENHANCED_H

#include "lexer.h"

namespace disp {

// Extended token types for FFI and web features
enum class ExtendedTokenType {
    // FFI keywords
    EXTERN,           // extern "C" { ... }
    LIBRARY,          // library("libname")
    FUNCTION_DECL,    // func_decl(type, params)
    CALLBACK,         // callback type
    UNSAFE,           // unsafe { ... }
    
    // Web keywords
    REST,             // rest("GET", "/path")
    GRAPHQL,          // graphql("query { ... }")
    WEBSOCKET,        // websocket("wss://...")
    JSON,             // json { ... }
    FETCH,            // fetch("url")
    SERVER,           // server(port)
    ROUTE,            // route("/path", handler)
    
    // Module system
    MODULE,           // module("name")
    IMPORT,           // import("module")
    EXPORT,           // export func
    
    // async/await
    ASYNC,            // async
    AWAIT,            // await
    PROMISE,          // promise
    
    // Error handling
    TRY,              // try
    CATCH,            // catch
    FINALLY,          // finally
    THROW,            // throw
    
    // Type annotations
    OPTIONAL,         // Type?
    NULLABLE,         // Type!
    GENERIC,          // Type<T>
    UNION,            // Type1 | Type2
    
    // Testing
    TEST,             // test("name")
    ASSERT,           // assert(cond)
    BENCH,            // bench("name")
};

// Enhanced token with extended types
struct ExtendedToken : public Token {
    ExtendedTokenType extType;
    
    ExtendedToken() : extType((ExtendedTokenType)-1) {}
};

// FFI function declaration
struct FFIFunctionDecl {
    std::string name;
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters;  // (name, type)
    std::string library;
    bool isVarArgs;
};

// Enhanced Lexer
class EnhancedLexer : public Lexer {
public:
    using Lexer::Lexer;  // Inherit constructor
    
    // Scan extended tokens
    ExtendedToken scanExtendedToken() {
        ExtendedToken token;
        Token base = nextToken();
        token.type = base.type;
        token.lexeme = base.lexeme;
        token.line = base.line;
        token.column = base.column;
        
        // Detect extended keywords
        if (base.type == TokenType::IDENT) {
            if (base.lexeme == "extern") token.extType = ExtendedTokenType::EXTERN;
            else if (base.lexeme == "library") token.extType = ExtendedTokenType::LIBRARY;
            else if (base.lexeme == "unsafe") token.extType = ExtendedTokenType::UNSAFE;
            else if (base.lexeme == "rest") token.extType = ExtendedTokenType::REST;
            else if (base.lexeme == "graphql") token.extType = ExtendedTokenType::GRAPHQL;
            else if (base.lexeme == "websocket") token.extType = ExtendedTokenType::WEBSOCKET;
            else if (base.lexeme == "json") token.extType = ExtendedTokenType::JSON;
            else if (base.lexeme == "fetch") token.extType = ExtendedTokenType::FETCH;
            else if (base.lexeme == "server") token.extType = ExtendedTokenType::SERVER;
            else if (base.lexeme == "route") token.extType = ExtendedTokenType::ROUTE;
            else if (base.lexeme == "module") token.extType = ExtendedTokenType::MODULE;
            else if (base.lexeme == "import") token.extType = ExtendedTokenType::IMPORT;
            else if (base.lexeme == "export") token.extType = ExtendedTokenType::EXPORT;
            else if (base.lexeme == "async") token.extType = ExtendedTokenType::ASYNC;
            else if (base.lexeme == "await") token.extType = ExtendedTokenType::AWAIT;
            else if (base.lexeme == "promise") token.extType = ExtendedTokenType::PROMISE;
            else if (base.lexeme == "try") token.extType = ExtendedTokenType::TRY;
            else if (base.lexeme == "catch") token.extType = ExtendedTokenType::CATCH;
            else if (base.lexeme == "finally") token.extType = ExtendedTokenType::FINALLY;
            else if (base.lexeme == "throw") token.extType = ExtendedTokenType::THROW;
            else if (base.lexeme == "test") token.extType = ExtendedTokenType::TEST;
            else if (base.lexeme == "assert") token.extType = ExtendedTokenType::ASSERT;
            else if (base.lexeme == "bench") token.extType = ExtendedTokenType::BENCH;
        }
        
        return token;
    }
};

} // namespace disp

#endif // DISPLEXITY_LEXER_ENHANCED_H
