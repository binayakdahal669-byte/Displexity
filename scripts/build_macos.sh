#!/bin/bash
# Displexity macOS Cross-Compile Script
# Requires: osxcross or native macOS with clang

set -e

INPUT_FILE="$1"
OUTPUT_FILE="$2"

if [ -z "$INPUT_FILE" ]; then
    echo "Usage: build_macos.sh <input.dis> [output]"
    exit 1
fi

BASENAME=$(basename "$INPUT_FILE" .dis)
OUTPUT_FILE="${OUTPUT_FILE:-${BASENAME}_macos}"

echo "Cross-compiling $INPUT_FILE to macOS..."

# Generate C code
disp "$INPUT_FILE" -emit-c -o "${BASENAME}_temp.c" -no-log

# Check for cross-compiler
if [ "$(uname)" = "Darwin" ]; then
    # Native macOS
    CROSS_CC="clang"
    CROSS_FLAGS="-arch x86_64 -arch arm64"
elif command -v o64-clang &> /dev/null; then
    # osxcross x86_64
    CROSS_CC="o64-clang"
    CROSS_FLAGS=""
elif command -v oa64-clang &> /dev/null; then
    # osxcross arm64
    CROSS_CC="oa64-clang"
    CROSS_FLAGS=""
elif [ -n "$OSXCROSS_ROOT" ]; then
    CROSS_CC="$OSXCROSS_ROOT/target/bin/o64-clang"
    CROSS_FLAGS=""
else
    echo "Error: No macOS cross-compiler found"
    echo "Options:"
    echo "  1. Run on macOS with Xcode/clang installed"
    echo "  2. Install osxcross: https://github.com/tpoechtrager/osxcross"
    echo "  3. Set OSXCROSS_ROOT environment variable"
    rm -f "${BASENAME}_temp.c"
    exit 1
fi

# Compile
$CROSS_CC $CROSS_FLAGS "${BASENAME}_temp.c" -o "$OUTPUT_FILE"

# Cleanup
rm -f "${BASENAME}_temp.c"

echo "Created: $OUTPUT_FILE"
if [ "$(uname)" = "Darwin" ]; then
    echo "Universal binary (x86_64 + arm64)"
fi
