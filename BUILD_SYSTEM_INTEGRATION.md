# Displexity Build System Integration

## CMakeLists.txt Updates

If you're using CMake (most likely):

```cmake
# In CMakeLists.txt, add these source files:

set(DISPLEXITY_SOURCES
    src/main.cpp
    src/lexer.h
    src/parser.h
    src/codegen.h
    src/ast.h
    # NEW FILES:
    src/interop.h
    src/webfeatures.h
    src/lexer_enhanced.h
    src/codegen_extended.h
    src/runtime/runtime.h
    src/runtime/runtime_enhanced.h
)

# Add these include directories:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/runtime)

# Link required libraries for web/socket features:
target_link_libraries(displexity_compiler
    # FFI support
    ${CMAKE_DL_LIBS}  # dlopen/dlclose on Unix
    
    # Web support
    curl              # HTTP/REST
    
    # Threading for async
    Threads::Threads
)

# Windows specific
if(WIN32)
    target_link_libraries(displexity_compiler
        Ws2_32        # Windows Sockets
    )
endif()

# Feature flags for conditional compilation:
target_compile_definitions(displexity_compiler PUBLIC
    DISPLEXITY_FFI_ENABLED=1
    DISPLEXITY_WEB_ENABLED=1
    DISPLEXITY_ASYNC_ENABLED=1
    DISPLEXITY_ERRORS_ENABLED=1
    DISPLEXITY_TESTS_ENABLED=1
)
```

## Makefile Updates (if using Make)

```makefile
# Compiler sources
SOURCES = src/main.cpp

# Header-only files (will be compiled when included)
HEADERS = \
    src/lexer.h \
    src/parser.h \
    src/codegen.h \
    src/ast.h \
    src/interop.h \
    src/webfeatures.h \
    src/lexer_enhanced.h \
    src/codegen_extended.h \
    src/runtime/runtime.h \
    src/runtime/runtime_enhanced.h

# Output
EXECUTABLE = disp

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra
CXXFLAGS += -DDISPLEXITY_FFI_ENABLED=1
CXXFLAGS += -DDISPLEXITY_WEB_ENABLED=1
CXXFLAGS += -DDISPLEXITY_ASYNC_ENABLED=1
CXXFLAGS += -DDISPLEXITY_ERRORS_ENABLED=1
CXXFLAGS += -DDISPLEXITY_TESTS_ENABLED=1

# Include paths
CXXFLAGS += -Isrc -Isrc/runtime

# Libraries
LDFLAGS = -ldl -lcurl -pthread
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lws2_32
endif

# Build target
$(EXECUTABLE): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS)

clean:
	rm -f $(EXECUTABLE) *.o

.PHONY: all clean
```

## build.bat Updates (Windows Batch)

```batch
@echo off
REM Compile with all enhancements

set COMPILER=cl
set CXXFLAGS=/std:c++17 /W4 /EHsc
set INCLUDES=/I src /I src/runtime
set DEFINES=/D DISPLEXITY_FFI_ENABLED=1 /D DISPLEXITY_WEB_ENABLED=1 /D DISPLEXITY_ASYNC_ENABLED=1 /D DISPLEXITY_ERRORS_ENABLED=1 /D DISPLEXITY_TESTS_ENABLED=1
set LIBS=ws2_32.lib

%COMPILER% src/main.cpp %CXXFLAGS% %INCLUDES% %DEFINES% /Fe:disp.exe /link %LIBS%

if %ERRORLEVEL% EQU 0 (
    echo Build successful: disp.exe
) else (
    echo Build failed
    exit /b 1
)
```

## build.sh Updates (Unix/Linux)

```bash
#!/bin/bash

# Compiler
COMPILER=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -fPIC"
INCLUDES="-Isrc -Isrc/runtime"
DEFINES="-DDISPLEXITY_FFI_ENABLED=1 -DDISPLEXITY_WEB_ENABLED=1 -DDISPLEXITY_ASYNC_ENABLED=1 -DDISPLEXITY_ERRORS_ENABLED=1 -DDISPLEXITY_TESTS_ENABLED=1"
LIBS="-ldl -lcurl -pthread"

# Compile
$COMPILER src/main.cpp $CXXFLAGS $INCLUDES $DEFINES -o disp $LIBS

if [ $? -eq 0 ]; then
    echo "Build successful: disp"
else
    echo "Build failed"
    exit 1
fi
```

## Header Integration in main.cpp

```cpp
#include "src/lexer.h"
#include "src/parser.h"
#include "src/codegen.h"
#include "src/ast.h"

// NEW - Enhanced features
#include "src/interop.h"
#include "src/webfeatures.h"
#include "src/lexer_enhanced.h"
#include "src/codegen_extended.h"
#include "src/runtime/runtime_enhanced.h"

#if DISPLEXITY_FFI_ENABLED
#include "src/runtime/displexity_ffi.h"
#endif

#if DISPLEXITY_WEB_ENABLED
#include "src/runtime/displexity_web.h"
#endif

#if DISPLEXITY_ASYNC_ENABLED
#include "src/runtime/displexity_async.h"
#endif

#if DISPLEXITY_ERRORS_ENABLED
#include "src/runtime/displexity_errors.h"
#endif

// ... rest of main.cpp
```

## Adding Feature Detection to CodeGen

In `src/codegen.h`, modify the `scanFeatures()` method:

```cpp
class CodeGen {
private:
    bool needsFFI = false;
    bool needsWeb = false;
    bool needsAsync = false;
    bool needsErrors = false;
    bool needsTests = false;
    bool needsModules = false;

public:
    void scanFeatures(ASTNode* ast) {
        // Scan for FFI usage
        if (containsExternDeclaration(ast)) {
            needsFFI = true;
        }
        
        // Scan for web features
        if (containsRESTCall(ast) || containsWebSocket(ast) || 
            containsGraphQL(ast) || containsJSON(ast)) {
            needsWeb = true;
        }
        
        // Scan for async
        if (containsAsyncAwait(ast)) {
            needsAsync = true;
        }
        
        // Scan for error handling
        if (containsTryCatch(ast) || containsThrow(ast)) {
            needsErrors = true;
        }
        
        // Scan for tests
        if (containsTestBlock(ast)) {
            needsTests = true;
        }
    }
    
    void emitHeaders(std::ostream& out) {
        // Standard headers
        out << "#include <stdio.h>\n";
        out << "#include <stdlib.h>\n";
        out << "#include <string.h>\n";
        
        // Feature-specific headers
        if (needsFFI) {
            out << "#include \"displexity_ffi.h\"\n";
            out << "#include \"displexity_interop.h\"\n";
        }
        
        if (needsWeb) {
            out << "#include \"displexity_web.h\"\n";
            out << "#include \"displexity_json.h\"\n";
        }
        
        if (needsAsync) {
            out << "#include \"displexity_async.h\"\n";
        }
        
        if (needsErrors) {
            out << "#include \"displexity_errors.h\"\n";
        }
        
        if (needsTests) {
            out << "#include \"displexity_test.h\"\n";
        }
        
        out << "\n";
    }
    
    void emitInitialization(std::ostream& out) {
        out << "void displexity_init() {\n";
        
        if (needsAsync) {
            out << "    dis_event_loop_init();\n";
        }
        
        if (needsTests) {
            out << "    dis_test_runner_init();\n";
        }
        
        out << "}\n\n";
        
        out << "void displexity_cleanup() {\n";
        
        if (needsFFI) {
            out << "    dis_libraries_cleanup();\n";
        }
        
        if (needsAsync) {
            out << "    dis_event_loop_cleanup();\n";
        }
        
        out << "}\n\n";
    }
};
```

## Conditional Compilation Flags

Create `src/features.h`:

```cpp
#ifndef DISPLEXITY_FEATURES_H
#define DISPLEXITY_FEATURES_H

// Feature compilation flags
#define DISPLEXITY_FFI_ENABLED 1
#define DISPLEXITY_WEB_ENABLED 1
#define DISPLEXITY_ASYNC_ENABLED 1
#define DISPLEXITY_ERRORS_ENABLED 1
#define DISPLEXITY_TESTS_ENABLED 1
#define DISPLEXITY_MODULES_ENABLED 1
#define DISPLEXITY_UNSAFE_ENABLED 1

// Feature checks
#if DISPLEXITY_FFI_ENABLED
    #define IF_FFI(x) x
#else
    #define IF_FFI(x)
#endif

#if DISPLEXITY_WEB_ENABLED
    #define IF_WEB(x) x
#else
    #define IF_WEB(x)
#endif

#if DISPLEXITY_ASYNC_ENABLED
    #define IF_ASYNC(x) x
#else
    #define IF_ASYNC(x)
#endif

#if DISPLEXITY_ERRORS_ENABLED
    #define IF_ERRORS(x) x
#else
    #define IF_ERRORS(x)
#endif

#if DISPLEXITY_TESTS_ENABLED
    #define IF_TESTS(x) x
#else
    #define IF_TESTS(x)
#endif

#endif // DISPLEXITY_FEATURES_H
```

## Runtime Library Dependencies

### Linux/Mac
```bash
# FFI support (included in standard C library)
# Already included

# Web support
sudo apt-get install libcurl4-openssl-dev
# or
brew install curl

# Threading
# Included in libc

# Socket
# Included in libc
```

### Windows
```batch
# Visual Studio includes all required headers
# vcpkg install curl:x64-windows
```

## Compiler Command Examples

### Basic Compilation
```bash
disp.exe main.dis -o main.disp
```

### With FFI
```bash
disp.exe main.dis --ffi -o main.disp
```

### With Web Features
```bash
disp.exe main.dis --web -o main.disp
```

### With Async
```bash
disp.exe main.dis --async -o main.disp
```

### All Features
```bash
disp.exe main.dis --full-featured -o main.disp
```

### With Specific Libraries
```bash
disp.exe main.dis --libs libm,libcurl,libsqlite3 -o main.disp
```

### Debug Build
```bash
disp.exe main.dis --debug --keep-temp -o main_debug.disp
```

## Compiler Flag Additions

In `src/main.cpp`, add command-line parsing:

```cpp
int main(int argc, char** argv) {
    // ... existing code ...
    
    // New command-line flags
    bool enableFFI = false;
    bool enableWeb = false;
    bool enableAsync = false;
    bool enableErrors = false;
    bool enableTests = false;
    
    std::vector<std::string> linkedLibraries;
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--ffi") enableFFI = true;
        if (arg == "--web") enableWeb = true;
        if (arg == "--async") enableAsync = true;
        if (arg == "--errors") enableErrors = true;
        if (arg == "--tests") enableTests = true;
        if (arg == "--full-featured") {
            enableFFI = enableWeb = enableAsync = enableErrors = enableTests = true;
        }
        if (arg == "--libs") {
            // Parse library list
            if (i + 1 < argc) {
                std::string libs = argv[++i];
                // Split by comma
                linkedLibraries = splitString(libs, ",");
            }
        }
    }
    
    // ... continue compilation ...
}
```

## Verification Checklist

- [ ] All headers compile without errors
- [ ] CMake/Make build system includes new files
- [ ] Feature flags are defined correctly
- [ ] External libraries (curl, pthread) link successfully
- [ ] Test programs compile and run
- [ ] FFI test calls external math library
- [ ] REST call can reach external service
- [ ] JSON parsing works correctly
- [ ] Error handling with try/catch functions
- [ ] Async/await execution works
- [ ] Test framework runs tests

## Build Test Script

Create `test_build.sh`:

```bash
#!/bin/bash

echo "Testing Displexity Build System..."

# Clean
make clean

# Build standard
echo "Building standard compiler..."
make
if [ $? -ne 0 ]; then
    echo "Standard build failed"
    exit 1
fi

# Test FFI
echo "Testing FFI compilation..."
./disp test_ffi.dis --ffi -o test_ffi.disp
if [ $? -ne 0 ]; then
    echo "FFI test failed"
    exit 1
fi

# Test Web
echo "Testing Web features..."
./disp test_web.dis --web -o test_web.disp
if [ $? -ne 0 ]; then
    echo "Web test failed"
    exit 1
fi

# Test Async
echo "Testing Async features..."
./disp test_async.dis --async -o test_async.disp
if [ $? -ne 0 ]; then
    echo "Async test failed"
    exit 1
fi

# Test Errors
echo "Testing Error handling..."
./disp test_errors.dis --errors -o test_errors.disp
if [ $? -ne 0 ]; then
    echo "Error test failed"
    exit 1
fi

echo "All build tests passed!"
```

---

## Summary

The build system integration is designed to:

1. **Modular** - Each feature can be enabled/disabled independently
2. **Platform-agnostic** - Works on Windows, Linux, macOS
3. **Flexible** - Supports CMake, Make, and batch scripts
4. **Clean** - Feature flags keep code organized
5. **Efficient** - Only links required libraries
6. **Testable** - Includes test suite

Start with your existing build system and add the enhancements incrementally.
