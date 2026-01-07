#!/bin/bash
# Displexity Debian Package Builder
# Creates .deb package from Displexity source

set -e

INPUT_FILE="$1"
PACKAGE_NAME="$2"
VERSION="${3:-1.0.0}"

if [ -z "$INPUT_FILE" ] || [ -z "$PACKAGE_NAME" ]; then
    echo "Usage: build_deb.sh <input.dis> <package-name> [version]"
    exit 1
fi

BASENAME=$(basename "$INPUT_FILE" .dis)
BUILD_DIR="deb_build_${PACKAGE_NAME}"

echo "Building Debian package: ${PACKAGE_NAME}_${VERSION}.deb"

# Create directory structure
mkdir -p "$BUILD_DIR/DEBIAN"
mkdir -p "$BUILD_DIR/usr/bin"
mkdir -p "$BUILD_DIR/usr/share/doc/$PACKAGE_NAME"

# Compile the binary
disp "$INPUT_FILE" -emit-exe -o "$BUILD_DIR/usr/bin/$PACKAGE_NAME" -no-log

# Create control file
cat > "$BUILD_DIR/DEBIAN/control" << EOF
Package: $PACKAGE_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Maintainer: Displexity User <user@example.com>
Description: Application built with Displexity
 This package was created using the Displexity compiler.
EOF

# Create copyright file
cat > "$BUILD_DIR/usr/share/doc/$PACKAGE_NAME/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: $PACKAGE_NAME
Source: Built with Displexity Compiler

Files: *
Copyright: $(date +%Y) Displexity User
License: MIT
EOF

# Build the package
dpkg-deb --build "$BUILD_DIR" "${PACKAGE_NAME}_${VERSION}_amd64.deb"

# Cleanup
rm -rf "$BUILD_DIR"

echo "Created: ${PACKAGE_NAME}_${VERSION}_amd64.deb"
