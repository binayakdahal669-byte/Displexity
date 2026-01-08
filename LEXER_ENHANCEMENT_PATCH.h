/*
 * Lexer Enhancements Patch
 * Add these to src/lexer.h TokenType enum and scanToken() method
 */

// ADD TO TokenType ENUM (around line 50):
// ============================================

enum class TokenType {
    // ... existing tokens ...
    
    // FFI & Interop Keywords
    EXTERN,           // external function declaration
    LIBRARY,          // library binding
    UNSAFE,           // unsafe block
    
    // Web & Network Keywords
    REST,             // REST API call
    GRAPHQL,          // GraphQL query
    WEBSOCKET,        // WebSocket connection
    JSON,             // JSON literal/type
    FETCH,            // HTTP fetch
    SERVER,           // HTTP server definition
    ROUTE,            // Route handler
    
    // Async Keywords
    ASYNC,            // async function
    AWAIT,            // await expression
    PROMISE,          // promise type
    THEN,             // promise.then()
    
    // Error Handling Keywords
    TRY,              // try block
    CATCH,            // catch block
    FINALLY,          // finally block
    THROW,            // throw error
    ERROR,            // error type
    
    // Testing Keywords
    TEST,             // test block
    ASSERT,           // assert statement
    BENCH,            // benchmark
    EXPECT,           // expect(value)
    
    // Module Keywords
    MODULE,           // module declaration
    EXPORT,           // export symbol
    IMPORT,           // import module
    
    // Type Keywords (Extended)
    OPTIONAL,         // T? (nullable type)
    UNION,            // T1 | T2 union type
};

// ADD TO scanToken() METHOD (around line 200):
// ============================================

Token Lexer::scanToken() {
    // ... existing code ...
    
    // After handling keywords, add recognition for new keywords:
    
    // Check for FFI keywords
    if (current_token == "extern") return makeToken(TokenType::EXTERN);
    if (current_token == "library") return makeToken(TokenType::LIBRARY);
    if (current_token == "unsafe") return makeToken(TokenType::UNSAFE);
    
    // Check for web keywords
    if (current_token == "rest") return makeToken(TokenType::REST);
    if (current_token == "graphql") return makeToken(TokenType::GRAPHQL);
    if (current_token == "websocket") return makeToken(TokenType::WEBSOCKET);
    if (current_token == "json") return makeToken(TokenType::JSON);
    if (current_token == "fetch") return makeToken(TokenType::FETCH);
    if (current_token == "server") return makeToken(TokenType::SERVER);
    if (current_token == "route") return makeToken(TokenType::ROUTE);
    
    // Check for async keywords
    if (current_token == "async") return makeToken(TokenType::ASYNC);
    if (current_token == "await") return makeToken(TokenType::AWAIT);
    if (current_token == "promise") return makeToken(TokenType::PROMISE);
    if (current_token == "then") return makeToken(TokenType::THEN);
    
    // Check for error keywords
    if (current_token == "try") return makeToken(TokenType::TRY);
    if (current_token == "catch") return makeToken(TokenType::CATCH);
    if (current_token == "finally") return makeToken(TokenType::FINALLY);
    if (current_token == "throw") return makeToken(TokenType::THROW);
    if (current_token == "error") return makeToken(TokenType::ERROR);
    
    // Check for test keywords
    if (current_token == "test") return makeToken(TokenType::TEST);
    if (current_token == "assert") return makeToken(TokenType::ASSERT);
    if (current_token == "bench") return makeToken(TokenType::BENCH);
    if (current_token == "expect") return makeToken(TokenType::EXPECT);
    
    // Check for module keywords
    if (current_token == "module") return makeToken(TokenType::MODULE);
    if (current_token == "export") return makeToken(TokenType::EXPORT);
    if (current_token == "import") return makeToken(TokenType::IMPORT);
    
    // ... rest of scanToken() ...
}
