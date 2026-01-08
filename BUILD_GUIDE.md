# Displexity - Build & Installer Guide

## Project Status

This is the comprehensive Displexity Programming Language project with:
- ✅ Full-featured compiler (C/C++ to executable)
- ✅ Interactive web IDE with code runner
- ✅ Complete documentation (Getting Started, Language Guide, API Reference)
- ✅ Release folder structure (32-bit and 64-bit ready)
- ✅ NSIS installer scripts (x86 and x64)
- ✅ Language download integration for optional tools
- ✅ Icon generation and resource embedding

## Building the Installers

### Prerequisites

1. **NSIS (Nullsoft Scriptable Install System)**
   - Download: https://nsis.sourceforge.io/Download
   - Or install via MSYS2:
     ```bash
     pacman -S mingw-w64-x86_64-nsis
     ```

2. **GCC/G++ compiler** (for compiling Displexity itself)
   ```bash
   # Windows: Already provided in repo (src/disp.cpp, etc.)
   # Linux: sudo apt-get install g++
   # macOS: xcode-select --install
   ```

3. **Python 3.9+** (for icon generation)
   ```bash
   pip install Pillow  # For icon generation
   ```

### Build Steps

#### 1. Prepare Release Folder
```bash
# Windows
.\setup_release.bat

# Linux/macOS
bash setup_release.sh
```

This creates:
```
release/
├── bin/
│   ├── win32/    (32-bit executables)
│   └── win64/    (64-bit executables)
├── lib/displexity/
├── include/displexity/
├── docs/
├── examples/
├── resources/
└── languages/
```

#### 2. Compile the Compiler

**Windows:**
```bash
build.bat
```

**Linux/macOS:**
```bash
./build.sh
```

This produces:
- `bin/disp.exe` or `bin/dispcompiler` (64-bit compiler)
- Embedded icons and resources

#### 3. Build Installers

**Windows (using NSIS):**
```bash
# Build 64-bit installer
makensis displexity-x64.nsi
# Output: displexity-x64-installer.exe

# Build 32-bit installer
makensis displexity-x86.nsi
# Output: displexity-x86-installer.exe

# Or use the automated script:
.\build_installers.bat
```

**Expected Output:**
```
displexity-x64-installer.exe (~15-30 MB)
displexity-x86-installer.exe (~12-20 MB)
```

## Project Structure

```
displexity/
├── src/
│   ├── disp.cpp                    (Main compiler entry)
│   ├── compiler/
│   │   ├── lexer.cpp
│   │   ├── parser.cpp
│   │   └── codegen.cpp
│   ├── displexity_disll_loader.*   (Runtime .disll loader)
│   ├── disp.rc                     (Windows resources)
│   └── ...
├── bin/
│   ├── disp.exe                    (Compiled compiler)
│   ├── displexitycompiler.exe
│   └── ...
├── docs/
│   ├── index.html                  (Interactive website)
│   ├── GETTING_STARTED.md
│   ├── LANGUAGE_GUIDE.md
│   ├── API_REFERENCE.md
│   └── ...
├── release/                        (Distribution packages)
│   ├── bin/win32/
│   ├── bin/win64/
│   ├── lib/displexity/
│   ├── include/displexity/
│   └── ...
├── disp_programs/                  (Language installer integration)
│   ├── languages.nsh               (NSIS integration)
│   ├── download_all_windows.ps1
│   ├── download_linux.sh
│   └── ...
├── scripts/
│   ├── link_dislls.py             (Copy .disll libraries)
│   ├── generate_icons.py          (Create icon files)
│   └── ...
├── displexity-x86.nsi             (32-bit installer)
├── displexity-x64.nsi             (64-bit installer)
├── build.bat / build.sh           (Compiler build)
├── build_installers.bat           (NSIS batch builder)
└── setup_release.bat              (Release preparation)
```

## Installing from Source

### Method 1: Using the Installer (Recommended)
```bash
# Windows
displexity-x64-installer.exe  # Double-click or run

# Then restart terminal and verify:
disp --version
```

### Method 2: Manual Build & Install

**Windows:**
```bash
REM Build compiler
build.bat

REM Copy to system PATH
copy bin\disp.exe C:\Windows\System32\
copy bin\displexitycompiler.exe C:\Windows\System32\

REM Verify
disp --version
```

**Linux/macOS:**
```bash
# Build
./build.sh

# Install
sudo make install

# Verify
disp --version
```

## Using the Interactive Website

The project includes a full-featured web-based IDE at `docs/index.html`:

1. **Open in browser:**
   ```bash
   # Windows
   start docs\index.html
   
   # macOS
   open docs/index.html
   
   # Linux
   xdg-open docs/index.html
   ```

2. **Features:**
   - ✅ Code editor with syntax highlighting
   - ✅ Run code using system compiler (if installed)
   - ✅ Fallback JavaScript parser for instant feedback
   - ✅ Example programs (Hello World, loops, arrays, functions)
   - ✅ Complete documentation
   - ✅ API reference
   - ✅ Dark mode IDE theme

3. **Running Code:**
   - Select compiler: "System Compiler" or "JavaScript Parser"
   - Click "▶ Run Code"
   - View output in real-time

## Distributing Your Project

### Create Release Packages

**Windows EXE:**
```bash
# Automatic (using build system)
makensis displexity-x64.nsi
makensis displexity-x86.nsi

# Result: Two installer .exe files
```

**Source Distribution:**
```bash
# Create source tarball
cd ..
tar -czf displexity-src.tar.gz displexity/
```

### Upload to GitHub

1. Create release on GitHub: https://github.com/displexity/displexity/releases
2. Upload files:
   - `displexity-x64-installer.exe`
   - `displexity-x86-installer.exe`
   - `displexity-src.tar.gz` (optional)
3. Add release notes with installation instructions

## Development Guide

### Adding New Features

1. **Compiler Enhancement:**
   - Edit `src/compiler/lexer.cpp` or `parser.cpp`
   - Recompile: `build.bat`
   - Test with examples

2. **New Library:**
   - Create `src/lib_<name>.dis`
   - Compile to `.disll`: `disp -emit-lib src/lib_<name>.dis`
   - Place in `release/lib/displexity/`

3. **Documentation:**
   - Edit markdown files in `docs/`
   - Update `docs/index.html` for website changes

### Testing

```bash
# Test compiler on examples
cd examples
..\bin\disp.exe hello.dis -o hello.exe
hello.exe

# Test installer
displexity-x64-installer.exe  # Run in VM or test environment
```

## Troubleshooting

### NSIS Not Found
```bash
# Install NSIS
# Windows: https://nsis.sourceforge.io/Download
# Or via MSYS2: pacman -S mingw-w64-x86_64-nsis

# Add to PATH
# Then retry: makensis displexity-x64.nsi
```

### Compiler Build Fails
```bash
# Ensure GCC is installed
g++ --version

# Check dependencies
# - disp.cpp includes should exist
# - src/compiler/* files present

# Try cleaning first
# Windows: delete bin\ folder and rebuild
# Linux: make clean && ./build.sh
```

### Website Won't Load
```bash
# Ensure docs/index.html exists
ls docs/index.html

# Open in browser (not cmd.exe)
# Windows: start docs\index.html
# macOS: open docs/index.html
# Linux: firefox docs/index.html
```

### Installer Components Missing

If the installer is missing files during build:
1. Run `setup_release.bat` to populate `release/` folder
2. Copy your built compiler to `release/bin/win64/`
3. Copy docs/examples to `release/` structure
4. Run `makensis displexity-x64.nsi` again

## Support

- **Documentation:** https://displexity.github.io (or `docs/index.html`)
- **GitHub Issues:** https://github.com/displexity/displexity/issues
- **Examples:** `examples/` folder in repository

## License

MIT License - See LICENSE.txt for details

---

**Version:** 1.2.0  
**Last Updated:** January 2026  
**Status:** Production Ready ✅
