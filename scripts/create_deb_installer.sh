#!/bin/bash
# Displexity Debian Package Installer Creator
# Creates a .deb package for the Displexity compiler itself

set -e

VERSION="${1:-1.2.0}"
ARCH="${2:-amd64}"
BUILD_DIR="deb_installer_build"
PACKAGE_NAME="displexity"

echo "Creating Displexity Debian package v${VERSION}..."

# Clean and create directory structure
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/DEBIAN"
mkdir -p "$BUILD_DIR/usr/bin"
mkdir -p "$BUILD_DIR/usr/lib/displexity"
mkdir -p "$BUILD_DIR/usr/include/displexity"
mkdir -p "$BUILD_DIR/usr/share/doc/displexity"
mkdir -p "$BUILD_DIR/usr/share/displexity/examples"

# Copy binaries
if [ -f "bin/disp" ]; then
    cp bin/disp "$BUILD_DIR/usr/bin/"
elif [ -f "release/bin/disp" ]; then
    cp release/bin/disp "$BUILD_DIR/usr/bin/"
elif [ -f "release/bin/disp.exe" ]; then
    # Cross-compile for Linux if on Windows
    echo "Note: Windows binary found, attempting cross-compile..."
fi

# Create symlinks for aliases
cd "$BUILD_DIR/usr/bin"
ln -sf disp dis
ln -sf disp dih
ln -sf disp di
cd - > /dev/null

# Copy libraries
if [ -d "release/lib/displexity" ]; then
    cp release/lib/displexity/*.disll "$BUILD_DIR/usr/lib/displexity/" 2>/dev/null || true
fi

# Copy headers
if [ -d "release/include/displexity" ]; then
    cp release/include/displexity/*.dish "$BUILD_DIR/usr/include/displexity/" 2>/dev/null || true
    cp release/include/displexity/*.tuy "$BUILD_DIR/usr/include/displexity/" 2>/dev/null || true
fi

# Copy documentation
if [ -d "release/docs" ]; then
    cp release/docs/*.md "$BUILD_DIR/usr/share/doc/displexity/" 2>/dev/null || true
fi

# Copy examples
if [ -d "release/examples" ]; then
    cp release/examples/*.dis "$BUILD_DIR/usr/share/displexity/examples/" 2>/dev/null || true
fi

# Create control file
cat > "$BUILD_DIR/DEBIAN/control" << EOF
Package: $PACKAGE_NAME
Version: $VERSION
Section: devel
Priority: optional
Architecture: $ARCH
Depends: libc6 (>= 2.17), gcc
Maintainer: Displexity Team <team@displexity.dev>
Homepage: https://github.com/displexity
Description: Displexity Programming Language Compiler
 Displexity is a modern programming language designed for
 game development, system programming, and cross-platform
 applications. Features include:
  - Simple, clean syntax
  - TUI bytecode for portable terminal apps
  - Cross-platform compilation
  - Built-in graphics and networking support
EOF

# Create postinst script
cat > "$BUILD_DIR/DEBIAN/postinst" << 'EOF'
#!/bin/bash
set -e

# Set up environment
echo 'export DISPLEXITY_HOME=/usr' >> /etc/profile.d/displexity.sh
chmod +x /etc/profile.d/displexity.sh

# Update ldconfig
ldconfig

echo "Displexity installed successfully!"
echo "Run 'disp --help' to get started."
EOF
chmod 755 "$BUILD_DIR/DEBIAN/postinst"

# Create prerm script
cat > "$BUILD_DIR/DEBIAN/prerm" << 'EOF'
#!/bin/bash
set -e
rm -f /etc/profile.d/displexity.sh
EOF
chmod 755 "$BUILD_DIR/DEBIAN/prerm"

# Create copyright file
cat > "$BUILD_DIR/usr/share/doc/displexity/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: Displexity
Source: https://github.com/displexity

Files: *
Copyright: 2024-2026 Displexity Team
License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
EOF

# Build the package
dpkg-deb --build "$BUILD_DIR" "${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"

# Cleanup
rm -rf "$BUILD_DIR"

echo "Created: ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"
echo "Install with: sudo dpkg -i ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"
