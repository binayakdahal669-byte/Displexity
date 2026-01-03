#!/bin/bash

echo "Installing DISP Programming Language..."

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (use sudo)"
    exit 1
fi

# Build first if binaries don't exist
if [ ! -f "bin/dispcompiler" ] || [ ! -f "bin/dispide" ]; then
    echo "Building DISP first..."
    chmod +x build.sh
    ./build.sh
    
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi
fi

# Install binaries
echo "Installing binaries..."
cp bin/dispcompiler /usr/local/bin/
cp bin/dispide /usr/local/bin/
chmod +x /usr/local/bin/dispcompiler
chmod +x /usr/local/bin/dispide

# Create system directories
mkdir -p /usr/local/share/disp
mkdir -p /usr/local/share/disp/examples
mkdir -p /usr/local/share/disp/lib

# Install examples
echo "Installing examples..."
cp -r examples/* /usr/local/share/disp/examples/

# Create standard library directory
echo "Setting up standard library..."
echo '// DISP Standard Math Library' > /usr/local/share/disp/lib/math.dish
echo 'function sqrt(x) { return x ^ 0.5; }' >> /usr/local/share/disp/lib/math.dish
echo 'function pow(x, y) { return x ^ y; }' >> /usr/local/share/disp/lib/math.dish
echo 'function abs(x) { if (x < 0) return -x; else return x; }' >> /usr/local/share/disp/lib/math.dish

echo '// DISP Standard Network Library' > /usr/local/share/disp/lib/network.dish
echo 'function http_get(url) { http url "GET" response; return response; }' >> /usr/local/share/disp/lib/network.dish
echo 'function http_post(url, data) { http url "POST" response; return response; }' >> /usr/local/share/disp/lib/network.dish

# Add to PATH if not already there
if ! grep -q "/usr/local/bin" ~/.bashrc; then
    echo 'export PATH="/usr/local/bin:$PATH"' >> ~/.bashrc
fi

# Create desktop entry for IDE
echo "Creating desktop entry..."
cat > /usr/share/applications/dispide.desktop << EOF
[Desktop Entry]
Name=DISP IDE
Comment=Integrated Development Environment for DISP Programming Language
Exec=/usr/local/bin/dispide
Icon=text-editor
Terminal=true
Type=Application
Categories=Development;IDE;
EOF

echo "DISP Programming Language installed successfully!"
echo ""
echo "Usage:"
echo "  dispcompiler --help    - Show compiler help"
echo "  dispide               - Start the IDE"
echo "  dispcompiler hello.disp -o hello.dis  - Compile a program"
echo ""
echo "Examples are available in: /usr/local/share/disp/examples/"
echo "Standard libraries in: /usr/local/share/disp/lib/"
echo ""
echo "Restart your terminal or run 'source ~/.bashrc' to update PATH"