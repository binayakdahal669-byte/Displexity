#!/bin/bash
# Displexity Cross-Compile to Linux Script
# Requires: mingw-w64 or native Linux GCC

set -e

INPUT_FILE="$1"
OUTPUT_FILE="$2"

if [ -z "$INPUT_FILE" ]; then
    echo "Usage: build_linux.sh <input.dis> [output]"
    exit 1
fi

BASENAME=$(basename "$INPUT_FILE" .dis)
OUTPUT_FILE="${OUTPUT_FILE:-${BASENAME}_linux}"

echo "Cross-compiling $INPUT_FILE to Linux..."

# Generate C code
disp "$INPUT_FILE" -emit-c -o "${BASENAME}_temp.c" -no-log

# Check for cross-compiler
if command -v x86_64-linux-gnu-gcc &> /dev/null; then
    CROSS_CC="x86_64-linux-gnu-gcc"
elif command -v gcc &> /dev/null && [ "$(uname)" = "Linux" ]; then
    CROSS_CC="gcc"
else
    echo "Error: No Linux cross-compiler found"
    echo "Install mingw-w64 or run on Linux"
    rm -f "${BASENAME}_temp.c"
    exit 1
fi

# Compile
$CROSS_CC "${BASENAME}_temp.c" -o "$OUTPUT_FILE" -static

# Cleanup
rm -f "${BASENAME}_temp.c"

echo "Created: $OUTPUT_FILE"
