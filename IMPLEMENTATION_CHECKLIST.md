# Displexity Enhancement Implementation Checklist

## Pre-Implementation Setup

- [ ] Read `ENHANCEMENT_SUMMARY.md` - understand what's been added
- [ ] Review `DISPLEXITY_ENHANCED_FEATURES.md` - see all new language features
- [ ] Check `COMPILER_ENHANCEMENT_INTEGRATION.md` - understand architecture
- [ ] Review existing `src/lexer.h`, `src/parser.h`, `src/codegen.h` - understand current structure
- [ ] Set up version control - commit current working state before making changes
- [ ] Prepare test environment - have a test compiler binary ready

## Phase 1: Lexer Enhancement (30 minutes)

### Lexer Token Addition
- [ ] Open `src/lexer.h`
- [ ] Find `TokenType` enum (around line 50-150)
- [ ] Add new token types:
  - [ ] `EXTERN`, `LIBRARY`, `UNSAFE` (FFI)
  - [ ] `REST`, `GRAPHQL`, `WEBSOCKET`, `JSON`, `FETCH`, `SERVER`, `ROUTE` (Web)
  - [ ] `ASYNC`, `AWAIT`, `PROMISE`, `THEN` (Async)
  - [ ] `TRY`, `CATCH`, `FINALLY`, `THROW`, `ERROR` (Error handling)
  - [ ] `TEST`, `ASSERT`, `BENCH`, `EXPECT` (Testing)
  - [ ] `MODULE`, `EXPORT`, `IMPORT`, `AS` (Modules)
  - [ ] `OPTIONAL`, `UNION` (Type extensions)
- [ ] Use `LEXER_ENHANCEMENT_PATCH.h` as reference
- [ ] Compile and verify no new errors in lexer

### Keyword Recognition
- [ ] Find `scanToken()` method in `src/lexer.h`
- [ ] Add keyword recognition for each new token
- [ ] Pattern: `if (text == "keyword") return makeToken(TokenType::KEYWORD);`
- [ ] Test: Create simple test file with each keyword
- [ ] Verify lexer recognizes all new keywords

### Lexer Testing
- [ ] Create `test_lexer_tokens.dis` with all new keywords
- [ ] Run lexer in debug mode to verify tokens
- [ ] No lexer errors should occur

## Phase 2: AST Enhancement (1 hour)

### New AST Node Classes
- [ ] Open `src/ast.h`
- [ ] Add AST node classes from `PARSER_ENHANCEMENT_PATCH.h`:
  - [ ] `ExternDeclaration` (FFI)
  - [ ] `TryCatchBlock` (Error handling)
  - [ ] `RESTCall` (Web)
  - [ ] `AsyncFunctionCall` (Async)
  - [ ] `AwaitExpression` (Async)
  - [ ] `ThrowStatement` (Error handling)
  - [ ] `TestBlock` (Testing)
  - [ ] `WebSocketConnection` (Web)
  - [ ] `JSONLiteral` (Web)
  - [ ] `GraphQLQuery` (Web)
  - [ ] `ServerDefinition` (Web)

### Visitor Interface
- [ ] Update `ASTVisitor` interface in `src/ast.h`
- [ ] Add visit methods for each new node type
- [ ] Example: `virtual void visit(ExternDeclaration* node) = 0;`

### AST Testing
- [ ] Create simple AST nodes for each type
- [ ] Verify nodes compile without errors
- [ ] No memory issues or circular dependencies

## Phase 3: Parser Enhancement (2 hours)

### Parser Method Additions
- [ ] Open `src/parser.h`
- [ ] Add parse methods from `PARSER_ENHANCEMENT_PATCH.h`:
  - [ ] `parseExternDeclaration()` - FFI
  - [ ] `parseFFIFunction()`
  - [ ] `parseTryCatch()` - Error handling
  - [ ] `parseThrowStatement()`
  - [ ] `parseRESTCall()` - Web
  - [ ] `parseWebSocket()`
  - [ ] `parseJSON()`
  - [ ] `parseGraphQL()`
  - [ ] `parseServerDefinition()`
  - [ ] `parseAsyncAwait()` - Async
  - [ ] `parseTestBlock()` - Testing
  - [ ] `parseAssertStatement()`

### Parser Integration
- [ ] Modify `parseStatement()` to call new parse methods
  - [ ] Add `if (check(TokenType::EXTERN)) return parseExternDeclaration();`
  - [ ] Add similar checks for TRY, TEST, THROW
  
- [ ] Modify `parseExpression()` to call new expression parsers
  - [ ] Add checks for REST, WEBSOCKET, JSON, ASYNC, AWAIT, GRAPHQL

- [ ] Modify `parsePostfix()` or appropriate level
  - [ ] Add expression parsing calls

### Parser Testing
- [ ] Create `test_parser_extern.dis` - FFI syntax
- [ ] Create `test_parser_try.dis` - Try/catch syntax
- [ ] Create `test_parser_rest.dis` - REST syntax
- [ ] Create `test_parser_async.dis` - Async syntax
- [ ] Create `test_parser_test.dis` - Test syntax
- [ ] Parse each file and verify no parse errors
- [ ] AST should be generated correctly

## Phase 4: Code Generation Enhancement (2 hours)

### CodeGen Feature Detection
- [ ] Open `src/codegen.h`
- [ ] Add feature flag members:
  ```cpp
  bool needsFFI = false;
  bool needsWeb = false;
  bool needsAsync = false;
  bool needsErrors = false;
  bool needsTests = false;
  ```

- [ ] Update `scanFeatures()` method:
  - [ ] Detect `ExternDeclaration` → set `needsFFI = true`
  - [ ] Detect `RESTCall`, `WebSocket`, `JSON`, `GraphQL` → set `needsWeb = true`
  - [ ] Detect `AsyncFunctionCall`, `AwaitExpression` → set `needsAsync = true`
  - [ ] Detect `TryCatchBlock`, `ThrowStatement` → set `needsErrors = true`
  - [ ] Detect `TestBlock` → set `needsTests = true`

### CodeGen Header Emission
- [ ] Update `emitHeaders()` method:
  - [ ] If `needsFFI`: emit `#include "displexity_ffi.h"`
  - [ ] If `needsWeb`: emit `#include "displexity_web.h"`
  - [ ] If `needsAsync`: emit `#include "displexity_async.h"`
  - [ ] If `needsErrors`: emit `#include "displexity_errors.h"`
  - [ ] If `needsTests`: emit `#include "displexity_test.h"`

### CodeGen Visitor Methods
- [ ] Add visitor method for each AST node type:
  - [ ] `void visitExternDeclaration(ExternDeclaration* node)`
  - [ ] `void visitTryCatch(TryCatchBlock* node)`
  - [ ] `void visitRESTCall(RESTCall* node)`
  - [ ] Use `codegen_extended.h` as reference for implementation

### Code Generation Testing
- [ ] Compile `test_parser_extern.dis` → check generated C
- [ ] Compile `test_parser_try.dis` → check generated C
- [ ] Compile `test_parser_rest.dis` → check generated C
- [ ] Review generated C code for correctness
- [ ] Verify C code compiles with gcc/clang

## Phase 5: Runtime Implementation (1-2 hours)

### FFI Runtime
- [ ] Create `src/runtime/displexity_ffi.h`:
  - [ ] Implement `dis_library_open()` - dlopen/LoadLibrary
  - [ ] Implement `dis_library_close()` - dlclose/FreeLibrary
  - [ ] Implement `dis_library_get_function()` - dlsym/GetProcAddress
  - [ ] Include platform-specific headers
  - [ ] Test with math library (libm)

### Web Runtime
- [ ] Create `src/runtime/displexity_web.h`:
  - [ ] HTTP request implementation (using curl)
  - [ ] JSON parsing stubs
  - [ ] WebSocket implementation stubs

### Async Runtime
- [ ] Create `src/runtime/displexity_async.h`:
  - [ ] Promise creation/resolution
  - [ ] Event loop implementation
  - [ ] Thread pool management

### Error Runtime
- [ ] Create `src/runtime/displexity_errors.h`:
  - [ ] setjmp/longjmp wrapper
  - [ ] Error context management
  - [ ] Exception handling

### Test Runtime
- [ ] Create `src/runtime/displexity_test.h`:
  - [ ] Test registration
  - [ ] Assertion macros
  - [ ] Test runner

### Runtime Testing
- [ ] Compile FFI test with libm
- [ ] Run FFI test → verify math functions work
- [ ] Compile REST test → verify HTTP calls
- [ ] Compile error test → verify try/catch works
- [ ] Compile test framework → verify assertions work

## Phase 6: Build System Integration (1 hour)

### CMake Updates
- [ ] Open `CMakeLists.txt`
- [ ] Add new source files to `DISPLEXITY_SOURCES`
- [ ] Add include directories for `src/runtime`
- [ ] Link required libraries:
  - [ ] `${CMAKE_DL_LIBS}` for FFI
  - [ ] `curl` for web
  - [ ] `Threads::Threads` for async

### Compiler Flag Configuration
- [ ] Add feature compile definitions:
  ```cmake
  target_compile_definitions(displexity_compiler PUBLIC
      DISPLEXITY_FFI_ENABLED=1
      DISPLEXITY_WEB_ENABLED=1
      DISPLEXITY_ASYNC_ENABLED=1
      DISPLEXITY_ERRORS_ENABLED=1
      DISPLEXITY_TESTS_ENABLED=1
  )
  ```

### Build Testing
- [ ] `cmake ..` - configure
- [ ] `make` - build with new files
- [ ] Fix any compilation errors
- [ ] Verify compiler binary builds successfully

## Phase 7: Feature Testing (2-3 hours)

### FFI Testing
- [ ] Create `test_ffi_math.dis`:
  ```displexity
  extern "C" {
      library("libm")
      func sin(float x) -> float
  }
  float result = sin(0.0)
  print("sin(0) = " + result)
  ```
- [ ] Compile: `disp.exe test_ffi_math.dis --ffi -o test_ffi_math.disp`
- [ ] Run: `test_ffi_math.disp`
- [ ] Expected output: `sin(0) = 0.0` (approximately)

### REST API Testing
- [ ] Create `test_rest_simple.dis`:
  ```displexity
  rest("GET", "https://api.github.com") {
      headers {
          "Accept": "application/json"
      }
  }
  ```
- [ ] Compile: `disp.exe test_rest_simple.dis --web -o test_rest_simple.disp`
- [ ] Run: `test_rest_simple.disp`
- [ ] Expected: HTTP request succeeds

### JSON Testing
- [ ] Create `test_json_simple.dis`:
  ```displexity
  json obj = {
      "name": "test",
      "value": 42
  }
  ```
- [ ] Compile with `--web` flag
- [ ] Verify JSON parsing works

### Error Handling Testing
- [ ] Create `test_try_catch.dis`:
  ```displexity
  try {
      throw "test error"
  } catch (error) {
      print("caught error")
  }
  ```
- [ ] Compile with `--errors` flag
- [ ] Verify try/catch works

### Async Testing
- [ ] Create `test_async_simple.dis`:
  ```displexity
  async func getValue() {
      return 42
  }
  ```
- [ ] Compile with `--async` flag
- [ ] Verify async function declaration works

### Test Framework Testing
- [ ] Create `test_framework.dis`:
  ```displexity
  test("simple test") {
      assert(2 + 2 == 4)
  }
  ```
- [ ] Compile with `--tests` flag
- [ ] Run executable - should print test results

## Phase 8: Integration Testing (1-2 hours)

### Multi-Feature Testing
- [ ] Create test using FFI + Web:
  ```displexity
  extern "C" { library("libm") func sqrt(float x) -> float }
  rest("GET", "https://api.example.com")
  ```
- [ ] Compile with `--full-featured`
- [ ] Verify both features work together

### Complex Example Testing
- [ ] Compile `example_minecraft_3d.dis` (original game)
- [ ] Fix any remaining syntax issues
- [ ] Verify game compiles and runs

### Performance Testing
- [ ] Time compilation of large programs
- [ ] Profile runtime performance
- [ ] Check for memory leaks

## Phase 9: Documentation & Polish (1 hour)

### Documentation Review
- [ ] Review all .md files for accuracy
- [ ] Update with any changes made
- [ ] Add examples if needed

### Code Review
- [ ] Review all code for:
  - [ ] Proper error handling
  - [ ] Memory management
  - [ ] Performance
  - [ ] Code style consistency

### CI/CD Setup (Optional)
- [ ] Add automated build tests
- [ ] Set up test runner
- [ ] Configure continuous integration

## Verification Checklist

### Compilation
- [ ] Lexer compiles without errors ✓
- [ ] Parser compiles without errors ✓
- [ ] CodeGen compiles without errors ✓
- [ ] Runtime headers compile without errors ✓
- [ ] Full compiler builds successfully ✓

### Functionality
- [ ] FFI calls work with external libraries ✓
- [ ] REST API calls succeed ✓
- [ ] JSON parsing/generation works ✓
- [ ] Error handling with try/catch works ✓
- [ ] Async/await operations function ✓
- [ ] Test framework runs tests ✓
- [ ] WebSocket connections work ✓
- [ ] GraphQL queries execute ✓

### Performance
- [ ] Generated code runs at C speed ✓
- [ ] FFI calls have minimal overhead ✓
- [ ] No memory leaks ✓
- [ ] Compilation time reasonable ✓

### Compatibility
- [ ] Works on Windows ✓
- [ ] Works on Linux ✓
- [ ] Works on macOS ✓
- [ ] Existing code still compiles ✓

## Timeline

**Total Implementation Time: ~12-15 hours**

| Phase | Time | Status |
|-------|------|--------|
| 1. Lexer | 0.5h | ⬜ Not Started |
| 2. AST | 1h | ⬜ Not Started |
| 3. Parser | 2h | ⬜ Not Started |
| 4. CodeGen | 2h | ⬜ Not Started |
| 5. Runtime | 1-2h | ⬜ Not Started |
| 6. Build System | 1h | ⬜ Not Started |
| 7. Feature Testing | 2-3h | ⬜ Not Started |
| 8. Integration Testing | 1-2h | ⬜ Not Started |
| 9. Polish | 1h | ⬜ Not Started |

## Resources

### Reference Documentation
- `ENHANCEMENT_SUMMARY.md` - Overview of all changes
- `DISPLEXITY_ENHANCED_FEATURES.md` - Feature documentation
- `COMPILER_ENHANCEMENT_INTEGRATION.md` - Architecture guide
- `IMPLEMENTATION_GUIDE.md` - Step-by-step instructions
- `LEXER_ENHANCEMENT_PATCH.h` - Lexer code patch
- `PARSER_ENHANCEMENT_PATCH.h` - Parser code patch
- `BUILD_SYSTEM_INTEGRATION.md` - Build system setup
- `ENHANCEMENT_EXAMPLES.md` - Code examples

### External Resources
- GCC/Clang documentation - for compiling generated C
- CURL documentation - for HTTP/REST support
- OpenSSL documentation - for WebSocket/HTTPS
- POSIX threading - for async/await

## Success Criteria

✅ All compilation errors resolved
✅ All 8 feature types functional
✅ Generated C code compiles and runs
✅ Performance comparable to hand-written C
✅ Works on Windows/Linux/macOS
✅ Backward compatible with existing code
✅ Comprehensive test coverage
✅ Documentation complete and accurate

---

**Ready to begin? Start with Phase 1: Lexer Enhancement**

Use this checklist to track your progress. Mark items as complete as you finish them.

Good luck with the implementation! 🚀
