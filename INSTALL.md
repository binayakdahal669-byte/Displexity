# Displexity - Installation Instructions

## Quick Start

### Windows

1. **Download installer:**
   - 64-bit: `displexity-x64-installer.exe` (Recommended)
   - 32-bit: `displexity-x86-installer.exe`

2. **Run installer:**
   - Double-click the `.exe` file
   - Follow the installation wizard
   - Choose components:
     - ✓ Compiler (required)
     - ✓ Standard Libraries
     - ✓ IDE Configuration
     - ☐ Examples
     - ☐ Documentation
     - ☐ Optional Languages (GCC, Python, Node.js, etc.)

3. **Restart your terminal** (Command Prompt or PowerShell)

4. **Verify installation:**
   ```cmd
   disp --version
   ```

### macOS

1. **Build from source:**
   ```bash
   git clone https://github.com/displexity/displexity
   cd displexity
   ./build.sh
   sudo make install
   ```

2. **Verify:**
   ```bash
   disp --version
   ```

### Linux

1. **Build from source:**
   ```bash
   git clone https://github.com/displexity/displexity
   cd displexity
   ./build.sh
   sudo make install
   ```

2. **Verify:**
   ```bash
   disp --version
   ```

## Your First Program

Create a file named `hello.dis`:

```disp
function main() {
    print("Hello, World!");
    return 0;
}
```

Compile and run:

```bash
# Windows
disp hello.dis -o hello.exe
hello.exe

# macOS/Linux
disp hello.dis -o hello
./hello
```

Expected output:
```
Hello, World!
```

## Next Steps

1. **Read the getting started guide:**
   ```bash
   # Open in browser
   # Windows: start docs\index.html
   # macOS: open docs/index.html
   # Linux: firefox docs/index.html
   ```

2. **Try the online IDE:** Open `docs/index.html` in your browser

3. **Explore examples:**
   - `examples/hello.dis` - Hello World
   - `examples/game.dis` - Simple TUI game
   - `examples/graphics.dis` - 3D graphics demo

4. **Install optional languages:**
   - Run installer again and select language downloads
   - Or manually download from disp_programs/

## Troubleshooting

### "disp: command not found" (macOS/Linux)

Add to PATH:
```bash
export PATH="/usr/local/bin:$PATH"
source ~/.bashrc  # or ~/.zshrc
```

Or reinstall:
```bash
sudo make install
```

### "disp.exe is not recognized" (Windows)

1. Check if installed: Open `C:\Program Files\Displexity\bin\`
2. Restart Command Prompt or PowerShell
3. If still not found, add manually to PATH:
   - Right-click "This PC" → Properties
   - Advanced system settings
   - Environment variables
   - Add `C:\Program Files\Displexity\bin` to PATH

### Compiler won't build

Ensure you have:
- GCC/G++ installed: `g++ --version`
- C++17 support
- Python 3.9+ (for icon generation): `python --version`

### Installer fails with missing components

Run `setup_release.bat` before building installer:
```bash
cd displexity
.\setup_release.bat
.\build_installers.bat
```

## Optional Language Downloads

During installation, you can optionally download compilers/runtimes:

- **GCC/G++** - C/C++ compiler
- **Python** - Python 3.13+
- **Node.js** - JavaScript runtime
- **Java** - OpenJDK 21
- **Rust** - Rust toolchain
- **Go** - Go 1.23+
- **Zig** - Zig compiler
- **And 20+ more...**

These enable Displexity to:
1. Compile your `.dis` files using alternative backends
2. Call code from other languages via FFI
3. Integrate with multi-language projects

To add languages after installation:
```bash
# Windows
powershell -ExecutionPolicy Bypass -File disp_programs\download_all_windows.ps1

# Linux/macOS
bash disp_programs/download_linux.sh
```

## Uninstalling

### Windows
- Settings → Apps → Apps & features
- Find "Displexity"
- Click Uninstall

Or run: `C:\Program Files\Displexity\uninst.exe`

### macOS/Linux
```bash
sudo rm -rf /usr/local/bin/disp*
sudo rm -rf /usr/local/lib/displexity
```

## Getting Help

- **Documentation:** Open `docs/index.html` in your browser
- **GitHub Issues:** https://github.com/displexity/displexity/issues
- **Discussions:** https://github.com/displexity/displexity/discussions
- **Email:** hello@displexity.com

## System Requirements

- **Windows:** 7, 8, 10, or 11 (32-bit or 64-bit)
- **macOS:** 10.12+ (Intel or Apple Silicon)
- **Linux:** Any modern distribution (Ubuntu 18.04+, Fedora, etc.)
- **RAM:** 256 MB minimum, 1 GB recommended
- **Disk:** 500 MB for installation + optional languages

## License

Displexity is released under the MIT License.
See LICENSE.txt for details.

---

**Questions?** Check the [FAQ](docs/index.html) or file an issue on GitHub!
