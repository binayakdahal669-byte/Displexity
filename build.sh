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
g++ -std=c++17 -Wall -Wextra -O2 -Isrc $PLATFORM_FLAG \
    src/main.cpp \
    src/compiler/lexer.cpp \
    src/compiler/parser.cpp \
    src/compiler/codegen.cpp \
    src/utils/logger.cpp \
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