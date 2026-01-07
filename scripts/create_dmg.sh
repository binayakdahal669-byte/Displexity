#!/bin/bash
# Displexity macOS DMG Installer Creator
# Creates a .dmg disk image for macOS

set -e

VERSION="${1:-1.2.0}"
APP_NAME="Displexity"
DMG_NAME="Displexity-${VERSION}-macOS"
BUILD_DIR="dmg_build"

echo "Creating Displexity macOS DMG v${VERSION}..."

# Check if running on macOS
if [ "$(uname)" != "Darwin" ]; then
    echo "Warning: DMG creation works best on macOS"
    echo "Creating a tarball instead..."
    
    # Create tarball for non-macOS systems
    mkdir -p "$BUILD_DIR/$APP_NAME"
    cp -r release/bin "$BUILD_DIR/$APP_NAME/" 2>/dev/null || true
    cp -r release/lib "$BUILD_DIR/$APP_NAME/" 2>/dev/null || true
    cp -r release/include "$BUILD_DIR/$APP_NAME/" 2>/dev/null || true
    cp -r release/docs "$BUILD_DIR/$APP_NAME/" 2>/dev/null || true
    cp -r release/examples "$BUILD_DIR/$APP_NAME/" 2>/dev/null || true
    
    tar -czf "${DMG_NAME}.tar.gz" -C "$BUILD_DIR" "$APP_NAME"
    rm -rf "$BUILD_DIR"
    
    echo "Created: ${DMG_NAME}.tar.gz"
    echo "Note: Run this script on macOS to create a proper DMG"
    exit 0
fi

# Clean and create directory structure
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR/$APP_NAME"

# Create app bundle structure
APP_BUNDLE="$BUILD_DIR/$APP_NAME.app"
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Resources"
mkdir -p "$APP_BUNDLE/Contents/SharedSupport/lib/displexity"
mkdir -p "$APP_BUNDLE/Contents/SharedSupport/include/displexity"
mkdir -p "$APP_BUNDLE/Contents/SharedSupport/docs"
mkdir -p "$APP_BUNDLE/Contents/SharedSupport/examples"

# Copy binary
if [ -f "release/bin/disp" ]; then
    cp release/bin/disp "$APP_BUNDLE/Contents/MacOS/"
elif [ -f "bin/disp" ]; then
    cp bin/disp "$APP_BUNDLE/Contents/MacOS/"
fi
chmod +x "$APP_BUNDLE/Contents/MacOS/disp"

# Create launcher script
cat > "$APP_BUNDLE/Contents/MacOS/Displexity" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export DISPLEXITY_HOME="$SCRIPT_DIR/../SharedSupport"
exec "$SCRIPT_DIR/disp" "$@"
EOF
chmod +x "$APP_BUNDLE/Contents/MacOS/Displexity"

# Create Info.plist
cat > "$APP_BUNDLE/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>Displexity</string>
    <key>CFBundleIdentifier</key>
    <string>com.displexity.compiler</string>
    <key>CFBundleName</key>
    <string>Displexity</string>
    <key>CFBundleDisplayName</key>
    <string>Displexity</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    <key>CFBundleShortVersionString</key>
    <string>${VERSION}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>DISP</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeName</key>
            <string>Displexity Source</string>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>dis</string>
                <string>dish</string>
                <string>tuy</string>
            </array>
            <key>CFBundleTypeRole</key>
            <string>Editor</string>
        </dict>
    </array>
</dict>
</plist>
EOF

# Copy libraries
cp release/lib/displexity/*.disll "$APP_BUNDLE/Contents/SharedSupport/lib/displexity/" 2>/dev/null || true

# Copy headers
cp release/include/displexity/*.dish "$APP_BUNDLE/Contents/SharedSupport/include/displexity/" 2>/dev/null || true
cp release/include/displexity/*.tuy "$APP_BUNDLE/Contents/SharedSupport/include/displexity/" 2>/dev/null || true

# Copy docs
cp release/docs/*.md "$APP_BUNDLE/Contents/SharedSupport/docs/" 2>/dev/null || true

# Copy examples
cp release/examples/*.dis "$APP_BUNDLE/Contents/SharedSupport/examples/" 2>/dev/null || true

# Create command-line tools folder
mkdir -p "$BUILD_DIR/Command Line Tools"
cp "$APP_BUNDLE/Contents/MacOS/disp" "$BUILD_DIR/Command Line Tools/"
cd "$BUILD_DIR/Command Line Tools"
ln -sf disp dis
ln -sf disp dih
ln -sf disp di
cd - > /dev/null

# Create install script for CLI tools
cat > "$BUILD_DIR/Install Command Line Tools.command" << 'EOF'
#!/bin/bash
echo "Installing Displexity command line tools..."

# Create /usr/local/bin if it doesn't exist
sudo mkdir -p /usr/local/bin

# Copy tools
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
sudo cp "$SCRIPT_DIR/Command Line Tools/"* /usr/local/bin/
sudo chmod +x /usr/local/bin/disp

echo ""
echo "Displexity command line tools installed!"
echo "Run 'disp --help' to get started."
EOF
chmod +x "$BUILD_DIR/Install Command Line Tools.command"

# Create README
cat > "$BUILD_DIR/README.txt" << EOF
Displexity ${VERSION} for macOS
================================

Installation:
1. Drag Displexity.app to your Applications folder
2. Double-click "Install Command Line Tools.command" to install CLI tools

Usage:
- Open Terminal and run: disp --help
- Compile a program: disp hello.dis
- Create TUI bytecode: disp hello.dis -emit-tui

Documentation is available in the app bundle under:
Displexity.app/Contents/SharedSupport/docs/
EOF

# Create DMG
echo "Creating DMG..."
hdiutil create -volname "$APP_NAME" -srcfolder "$BUILD_DIR" -ov -format UDZO "${DMG_NAME}.dmg"

# Cleanup
rm -rf "$BUILD_DIR"

echo "Created: ${DMG_NAME}.dmg"
