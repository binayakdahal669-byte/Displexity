#!/bin/bash

echo "Building DISP Programming Language..."

# Create directories
mkdir -p bin build

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM_FLAG="-DLINUX"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM_FLAG="-DMACOS"
else
    PLATFORM_FLAG=""
fi

# Build compiler
echo "Building compiler..."
#!/usr/bin/env bash

# Optional: pass DISLLS as first argument or set DISLLS env var (comma-separated patterns)
if [ -n "$1" ] && [ -z "$DISLLS" ]; then
    DISLLS="$1"
fi

# If DISLLS provided, run helper to copy matching .disll files into bin/disll
if [ -n "$DISLLS" ]; then
    echo "Linking dislls: $DISLLS"
    python3 scripts/link_dislls.py "$DISLLS" || python scripts/link_dislls.py "$DISLLS"
fi

# Generate icons (logs, nvim) before building resources
echo "Generating icons..."
python3 scripts/generate_icons.py release/resources || python scripts/generate_icons.py release/resources || true

# Compile Windows resource (if windres is available)
if command -v windres >/dev/null 2>&1; then
    echo "Compiling resources with windres"
    windres src/disp.rc -O coff -o src/disp_res.o || true
    RES_OBJ=src/disp_res.o
else
    RES_OBJ=
fi

# Build the compiler (link resource object if present)
echo "Building compiler..."
g++ -std=c++17 -Wall -Wextra -O2 -Isrc $PLATFORM_FLAG \
    src/main.cpp \
    src/displexity_disll_loader.cpp \
    src/compiler/lexer.cpp \
    src/compiler/parser.cpp \
    src/compiler/codegen.cpp \
    src/utils/logger.cpp \
    $RES_OBJ \
    -o bin/dispcompiler

if [ $? -ne 0 ]; then
    echo "Compiler build failed!"
    exit 1
fi

# Build IDE
echo "Building IDE..."
g++ -std=c++17 -Wall -Wextra -O2 $PLATFORM_FLAG \
    ide/main.cpp \
    -o bin/dispide

if [ $? -ne 0 ]; then
    echo "IDE build failed!"
    exit 1
fi

echo "Build successful!"
echo
echo "To install system-wide:"
echo "  sudo cp bin/dispcompiler /usr/local/bin/"
echo "  sudo cp bin/dispide /usr/local/bin/"
echo "  sudo chmod +x /usr/local/bin/dispcompiler"
echo "  sudo chmod +x /usr/local/bin/dispide"
echo
echo "To test:"
echo "  ./bin/dispcompiler --help"
echo "  ./bin/dispide"
echo

# Make executables executable
chmod +x bin/dispcompiler
chmod +x bin/dispide