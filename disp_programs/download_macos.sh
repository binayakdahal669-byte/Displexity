#!/bin/bash
# Displexity Programming Languages Downloader - macOS
# Updated: January 2026

echo "============================================"
echo " Displexity Language Downloader - macOS"
echo "============================================"

mkdir -p macos/{dmg,pkg,tar,homebrew}

download() {
    echo "Downloading $1..."
    curl -L -o "$3" "$2" --progress-bar
}

echo "=== Downloading macOS Packages ==="

# PKG installers
download "Python 3.13.1" "https://www.python.org/ftp/python/3.13.1/python-3.13.1-macos11.pkg" "macos/pkg/python-3.13.1-macos11.pkg"
download "Node.js 22" "https://nodejs.org/dist/v22.12.0/node-v22.12.0.pkg" "macos/pkg/node-v22.12.0.pkg"
download "Go 1.23.4 x64" "https://go.dev/dl/go1.23.4.darwin-amd64.pkg" "macos/pkg/go1.23.4.darwin-amd64.pkg"
download "Go 1.23.4 ARM" "https://go.dev/dl/go1.23.4.darwin-arm64.pkg" "macos/pkg/go1.23.4.darwin-arm64.pkg"
download "R 4.4.2" "https://cran.r-project.org/bin/macosx/big-sur-arm64/base/R-4.4.2-arm64.pkg" "macos/pkg/R-4.4.2-arm64.pkg"

# DMG
download "Julia 1.11.1" "https://julialang-s3.julialang.org/bin/mac/x64/1.11/julia-1.11.1-mac64.dmg" "macos/dmg/julia-1.11.1-mac64.dmg"

# Tarballs
download "Rust" "https://sh.rustup.rs" "macos/tar/rustup-init.sh"
download "Zig 0.13.0 x64" "https://ziglang.org/download/0.13.0/zig-macos-x86_64-0.13.0.tar.xz" "macos/tar/zig-macos-x86_64-0.13.0.tar.xz"
download "Kotlin 2.0.21" "https://github.com/JetBrains/kotlin/releases/download/v2.0.21/kotlin-compiler-2.0.21.zip" "macos/tar/kotlin-compiler-2.0.21.zip"
download "Dart SDK" "https://storage.googleapis.com/dart-archive/channels/stable/release/3.5.4/sdk/dartsdk-macos-x64-release.zip" "macos/tar/dartsdk-macos-x64-release.zip"

# Homebrew script
cat > macos/homebrew/install_languages.sh << 'EOF'
#!/bin/bash
if ! command -v brew &> /dev/null; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi
brew install gcc llvm python node go ruby perl php lua rustup-init ghc erlang elixir r julia kotlin scala nim zig dart racket dmd vlang groovy dotnet-sdk fpc
rustup-init -y
echo "Done! All languages installed via Homebrew."
EOF
chmod +x macos/homebrew/install_languages.sh

echo "============================================"
echo " macOS Downloads Complete!"
echo "============================================"
