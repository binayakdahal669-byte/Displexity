#!/bin/bash
# Displexity Linux Tarball Installer Creator
# Creates a portable .tar.gz distribution for any Linux

set -e

VERSION="${1:-1.2.0}"
ARCH="${2:-x86_64}"
BUILD_DIR="displexity-${VERSION}-linux-${ARCH}"

echo "Creating Displexity Linux tarball v${VERSION}..."

# Clean and create directory structure
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/bin"
mkdir -p "$BUILD_DIR/lib/displexity"
mkdir -p "$BUILD_DIR/include/displexity"
mkdir -p "$BUILD_DIR/docs"
mkdir -p "$BUILD_DIR/examples"
mkdir -p "$BUILD_DIR/scripts"

# Copy binaries
if [ -f "bin/disp" ]; then
    cp bin/disp "$BUILD_DIR/bin/"
elif [ -f "release/bin/disp" ]; then
    cp release/bin/disp "$BUILD_DIR/bin/"
fi

# Create symlinks for aliases
cd "$BUILD_DIR/bin"
ln -sf disp dis
ln -sf disp dih
ln -sf disp di
cd - > /dev/null

# Copy libraries
cp release/lib/displexity/*.disll "$BUILD_DIR/lib/displexity/" 2>/dev/null || true

# Copy headers
cp release/include/displexity/*.dish "$BUILD_DIR/include/displexity/" 2>/dev/null || true
cp release/include/displexity/*.tuy "$BUILD_DIR/include/displexity/" 2>/dev/null || true

# Copy documentation
cp release/docs/*.md "$BUILD_DIR/docs/" 2>/dev/null || true
cp release/docs/html/* "$BUILD_DIR/docs/" 2>/dev/null || true

# Copy examples
cp release/examples/*.dis "$BUILD_DIR/examples/" 2>/dev/null || true

# Create install script
cat > "$BUILD_DIR/install.sh" << 'EOF'
#!/bin/bash
# Displexity Installation Script

set -e

PREFIX="${1:-/usr/local}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Installing Displexity to $PREFIX..."

# Check for root if installing to system directories
if [[ "$PREFIX" == "/usr"* ]] && [ "$EUID" -ne 0 ]; then
    echo "Error: Installing to $PREFIX requires root privileges"
    echo "Run: sudo ./install.sh"
    exit 1
fi

# Create directories
mkdir -p "$PREFIX/bin"
mkdir -p "$PREFIX/lib/displexity"
mkdir -p "$PREFIX/include/displexity"
mkdir -p "$PREFIX/share/doc/displexity"
mkdir -p "$PREFIX/share/displexity/examples"

# Install binaries
cp "$SCRIPT_DIR/bin/"* "$PREFIX/bin/"
chmod +x "$PREFIX/bin/disp"

# Install libraries
cp "$SCRIPT_DIR/lib/displexity/"* "$PREFIX/lib/displexity/" 2>/dev/null || true

# Install headers
cp "$SCRIPT_DIR/include/displexity/"* "$PREFIX/include/displexity/" 2>/dev/null || true

# Install docs
cp "$SCRIPT_DIR/docs/"* "$PREFIX/share/doc/displexity/" 2>/dev/null || true

# Install examples
cp "$SCRIPT_DIR/examples/"* "$PREFIX/share/displexity/examples/" 2>/dev/null || true

# Set up environment
if [ "$PREFIX" != "/usr" ] && [ "$PREFIX" != "/usr/local" ]; then
    echo ""
    echo "Add the following to your ~/.bashrc or ~/.zshrc:"
    echo "  export PATH=\"\$PATH:$PREFIX/bin\""
    echo "  export DISPLEXITY_HOME=\"$PREFIX\""
fi

echo ""
echo "Displexity installed successfully!"
echo "Run 'disp --help' to get started."
EOF
chmod +x "$BUILD_DIR/install.sh"

# Create uninstall script
cat > "$BUILD_DIR/uninstall.sh" << 'EOF'
#!/bin/bash
# Displexity Uninstallation Script

PREFIX="${1:-/usr/local}"

echo "Uninstalling Displexity from $PREFIX..."

# Check for root if uninstalling from system directories
if [[ "$PREFIX" == "/usr"* ]] && [ "$EUID" -ne 0 ]; then
    echo "Error: Uninstalling from $PREFIX requires root privileges"
    echo "Run: sudo ./uninstall.sh"
    exit 1
fi

# Remove files
rm -f "$PREFIX/bin/disp"
rm -f "$PREFIX/bin/dis"
rm -f "$PREFIX/bin/dih"
rm -f "$PREFIX/bin/di"
rm -rf "$PREFIX/lib/displexity"
rm -rf "$PREFIX/include/displexity"
rm -rf "$PREFIX/share/doc/displexity"
rm -rf "$PREFIX/share/displexity"

echo "Displexity uninstalled."
EOF
chmod +x "$BUILD_DIR/uninstall.sh"

# Create README
cat > "$BUILD_DIR/README.md" << EOF
# Displexity ${VERSION} for Linux

## Quick Installation

\`\`\`bash
# Install to /usr/local (requires sudo)
sudo ./install.sh

# Or install to custom location
./install.sh ~/displexity
\`\`\`

## Manual Installation

1. Copy \`bin/disp\` to a directory in your PATH
2. Set \`DISPLEXITY_HOME\` environment variable to this directory
3. Optionally copy libraries and headers

## Getting Started

\`\`\`bash
# Check installation
disp --version

# Compile a program
disp examples/hello.dis

# Run the compiled program
./hello

# Create TUI bytecode (portable)
disp examples/hello.dis -emit-tui
disp run hello.tui
\`\`\`

## Directory Structure

- \`bin/\` - Compiler and tools
- \`lib/displexity/\` - Standard libraries (.disll)
- \`include/displexity/\` - Header files (.dish, .tuy)
- \`docs/\` - Documentation
- \`examples/\` - Example programs

## Uninstallation

\`\`\`bash
sudo ./uninstall.sh
\`\`\`
EOF

# Create the tarball
tar -czf "${BUILD_DIR}.tar.gz" "$BUILD_DIR"

# Cleanup
rm -rf "$BUILD_DIR"

echo "Created: ${BUILD_DIR}.tar.gz"
echo "Extract with: tar -xzf ${BUILD_DIR}.tar.gz"
echo "Install with: cd ${BUILD_DIR} && sudo ./install.sh"
