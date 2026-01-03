#!/bin/bash
# Packaging script for all platforms
# Builds and packages disp compiler + IDE for Linux, Windows, macOS, Android (APK stub)

set -e

VERSION="0.1.0"
DIST="dist"

mkdir -p $DIST

echo "=== Displexity Packaging Script ==="

# Detect OS
OS=$(uname -s)
case "$OS" in
    Linux*)     OS_TYPE="linux" ;;
    Darwin*)    OS_TYPE="macos" ;;
    MINGW*)     OS_TYPE="windows" ;;
    *)          OS_TYPE="unknown" ;;
esac

echo "Detected OS: $OS_TYPE"

# Build for current platform
if [ "$OS_TYPE" = "linux" ] || [ "$OS_TYPE" = "macos" ]; then
    echo "Building for Unix-like system..."
    bash build.sh
    
    # Create tarball
    TARNAME="displexity-${VERSION}-${OS_TYPE}.tar.gz"
    tar -czf $DIST/$TARNAME -C build disp displexity-ide
    echo "Packaged: $DIST/$TARNAME"
    
elif [ "$OS_TYPE" = "windows" ]; then
    echo "Building for Windows..."
    cmd /c build.bat
    
    # Create ZIP
    ZIPNAME="displexity-${VERSION}-windows.zip"
    zip -r $DIST/$ZIPNAME build/disp.exe build/displexity-ide.exe
    echo "Packaged: $DIST/$ZIPNAME"
fi

echo ""
echo "APK stub (would require Qt or Android NDK):"
echo "  displexity-${VERSION}-android.apk (placeholder)"
echo ""
echo "Packaging complete!"
