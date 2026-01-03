#!/bin/bash
# Build script for Displexity compiler on Linux/macOS

set -e

echo "=== Building Displexity Compiler ==="

# Create build directory
mkdir -p build

# Compile disp
echo "Compiling disp..."
g++ -std=c++17 -O2 compiler/disp.cpp -o build/disp

# Compile IDE
echo "Compiling IDE..."
g++ -std=c++17 -O2 ide/main.cpp -o build/displexity-ide

echo "Build complete!"
echo "  Compiler: build/disp"
echo "  IDE: build/displexity-ide"

# Optionally run tests
if [ "$1" = "test" ]; then
    echo ""
    echo "Running tests..."
    
    ./build/disp examples/hello.dis -o build/hello.c
    gcc build/hello.c -o build/hello
    echo "Test: hello.dis"
    ./build/hello || true
    
    ./build/disp examples/math.dis -o build/math.c
    gcc build/math.c -o build/math
    echo "Test: math.dis"
    ./build/math || true
    
    ./build/disp examples/conditional.dis -o build/conditional.c
    gcc build/conditional.c -o build/conditional
    echo "Test: conditional.dis"
    ./build/conditional || true
fi
