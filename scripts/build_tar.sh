#!/bin/bash
# Displexity Linux Tarball Builder
# Creates .tar.gz archive with executable and documentation

set -e

INPUT_FILE="$1"
OUTPUT_NAME="${2:-displexity_app}"
VERSION="${3:-1.0.0}"

if [ -z "$INPUT_FILE" ]; then
    echo "Usage: build_tar.sh <input.dis> [output-name] [version]"
    exit 1
fi

BASENAME=$(basename "$INPUT_FILE" .dis)
BUILD_DIR="tar_build_${OUTPUT_NAME}"
ARCHIVE_NAME="${OUTPUT_NAME}-${VERSION}-linux-x64.tar.gz"

echo "Building Linux tarball: $ARCHIVE_NAME"

# Create directory structure
mkdir -p "$BUILD_DIR/$OUTPUT_NAME/bin"
mkdir -p "$BUILD_DIR/$OUTPUT_NAME/doc"
mkdir -p "$BUILD_DIR/$OUTPUT_NAME/examples"

# Compile the binary for Linux
if command -v x86_64-linux-gnu-gcc &> /dev/null; then
    CROSS_CC="x86_64-linux-gnu-gcc"
elif command -v gcc &> /dev/null && [ "$(uname)" = "Linux" ]; then
    CROSS_CC="gcc"
else
    echo "Warning: No Linux compiler found, using native compiler"
    CROSS_CC="gcc"
fi

# Generate C code and compile
disp "$INPUT_FILE" -emit-c -o "$BUILD_DIR/${BASENAME}_temp.c" -no-log
$CROSS_CC "$BUILD_DIR/${BASENAME}_temp.c" -o "$BUILD_DIR/$OUTPUT_NAME/bin/$OUTPUT_NAME" -static 2>/dev/null || \
$CROSS_CC "$BUILD_DIR/${BASENAME}_temp.c" -o "$BUILD_DIR/$OUTPUT_NAME/bin/$OUTPUT_NAME"

# Create README
cat > "$BUILD_DIR/$OUTPUT_NAME/README.md" << EOF
# $OUTPUT_NAME

Version: $VERSION
Built with Displexity Compiler

## Installation

1. Extract the archive:
   \`\`\`
   tar -xzf $ARCHIVE_NAME
   \`\`\`

2. Run the application:
   \`\`\`
   ./$OUTPUT_NAME/bin/$OUTPUT_NAME
   \`\`\`

3. (Optional) Add to PATH:
   \`\`\`
   sudo cp $OUTPUT_NAME/bin/$OUTPUT_NAME /usr/local/bin/
   \`\`\`

## License

Built with Displexity - https://github.com/displexity
EOF

# Copy source as example
cp "$INPUT_FILE" "$BUILD_DIR/$OUTPUT_NAME/examples/"

# Create the tarball
cd "$BUILD_DIR"
tar -czf "../$ARCHIVE_NAME" "$OUTPUT_NAME"
cd ..

# Cleanup
rm -rf "$BUILD_DIR"

echo "Created: $ARCHIVE_NAME"
echo "Extract with: tar -xzf $ARCHIVE_NAME"
