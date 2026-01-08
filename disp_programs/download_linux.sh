#!/bin/bash
# Displexity Programming Languages Downloader - Linux
# Updated: January 2026

echo "============================================"
echo " Displexity Language Downloader - Linux"
echo "============================================"

mkdir -p linux/{deb,rpm,tar}

download() {
    echo "Downloading $1..."
    curl -L -o "$3" "$2" --progress-bar
}

echo "=== Downloading Linux Packages ==="

# Tarballs
download "Go 1.23.4" "https://go.dev/dl/go1.23.4.linux-amd64.tar.gz" "linux/tar/go1.23.4.linux-amd64.tar.gz"
download "Node.js 22" "https://nodejs.org/dist/v22.12.0/node-v22.12.0-linux-x64.tar.xz" "linux/tar/node-v22.12.0-linux-x64.tar.xz"
download "Zig 0.13.0" "https://ziglang.org/download/0.13.0/zig-linux-x86_64-0.13.0.tar.xz" "linux/tar/zig-linux-x86_64-0.13.0.tar.xz"
download "Rust" "https://sh.rustup.rs" "linux/tar/rustup-init.sh"
download "Julia 1.11.1" "https://julialang-s3.julialang.org/bin/linux/x64/1.11/julia-1.11.1-linux-x86_64.tar.gz" "linux/tar/julia-1.11.1-linux-x86_64.tar.gz"
download "Kotlin 2.0.21" "https://github.com/JetBrains/kotlin/releases/download/v2.0.21/kotlin-compiler-2.0.21.zip" "linux/tar/kotlin-compiler-2.0.21.zip"
download "Nim 2.0.10" "https://nim-lang.org/download/nim-2.0.10-linux_x64.tar.xz" "linux/tar/nim-2.0.10-linux_x64.tar.xz"
download "Dart SDK" "https://storage.googleapis.com/dart-archive/channels/stable/release/3.5.4/sdk/dartsdk-linux-x64-release.zip" "linux/tar/dartsdk-linux-x64-release.zip"
download "V Language" "https://github.com/vlang/v/releases/latest/download/v_linux.zip" "linux/tar/v_linux.zip"

# Create apt install script
cat > linux/deb/install_languages.sh << 'EOF'
#!/bin/bash
sudo apt update
sudo apt install -y build-essential gcc g++ python3 python3-pip openjdk-21-jdk nodejs npm ruby perl php lua5.4 golang-go erlang elixir r-base gfortran fpc racket
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
echo "Done! For Zig, Nim, Julia, Kotlin, Dart - use tarballs in linux/tar/"
EOF
chmod +x linux/deb/install_languages.sh

# Create dnf install script
cat > linux/rpm/install_languages.sh << 'EOF'
#!/bin/bash
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y gcc gcc-c++ python3 python3-pip java-21-openjdk nodejs npm ruby perl php lua golang erlang elixir R gcc-gfortran fpc
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
echo "Done! For Zig, Nim, Julia, Kotlin, Dart - use tarballs in linux/tar/"
EOF
chmod +x linux/rpm/install_languages.sh

echo "============================================"
echo " Linux Downloads Complete!"
echo "============================================"
