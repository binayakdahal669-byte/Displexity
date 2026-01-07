# Cross-Platform Compiler Ecosystem - Design Document

## Overview

This document outlines the technical design for implementing a comprehensive cross-platform Displexity compiler ecosystem, including custom TUI executable format, cross-platform compilation targets, C transpiler, and IDE autocomplete.

## Architecture

### 1. Custom TUI Executable Format

The TUI (Terminal User Interface) executable format is a custom bytecode format that runs on any system without dependencies.

#### TUI Format Structure
```
[MAGIC:8]        "DISPLTUI" - identifies TUI format
[VERSION:4]      Format version (1)
[FLAGS:4]        Execution flags
[CHECKSUM:8]     Integrity verification
[TIMESTAMP:8]    Creation time
[BYTECODE_SIZE:8] Size of bytecode section
[DATA_SIZE:8]    Size of data section
[ENTRY_POINT:4]  Offset to main function
[BYTECODE]       TUI bytecode instructions
[DATA]           String literals, constants
```

#### TUI Bytecode Instructions
- `PUSH_INT`, `PUSH_STR`, `PUSH_FLOAT` - Push values
- `POP`, `DUP`, `SWAP` - Stack operations
- `ADD`, `SUB`, `MUL`, `DIV`, `MOD` - Arithmetic
- `EQ`, `NE`, `LT`, `GT`, `LE`, `GE` - Comparison
- `JMP`, `JZ`, `JNZ` - Control flow
- `CALL`, `RET` - Function calls
- `PRINT`, `INPUT` - I/O operations
- `HALT` - Program termination

#### TUI Runtime
A minimal interpreter embedded in the compiler that can execute TUI bytecode directly.

### 2. Default Output Format Change

#### Current Behavior
- `-o filename` outputs C code by default
- `-emit-exe` required for executable

#### New Behavior
- `-o filename` outputs `.disp` package by default
- `-emit-c` explicitly outputs C code
- `-emit-exe` outputs native executable
- `-emit-tui` outputs TUI bytecode executable

### 3. Cross-Platform Compilation Targets

#### Emit Modes
| Flag | Output | Description |
|------|--------|-------------|
| `-emit-c` | `.c` | C source code |
| `-emit-exe` | `.exe`/ELF | Native executable |
| `-emit-disp` | `.disp` | Displexity package |
| `-emit-tui` | `.tui` | TUI bytecode executable |
| `-emit-linux` | ELF | Linux executable (cross-compile) |
| `-emit-linux-deb` | `.deb` | Debian package |
| `-emit-linux-tar` | `.tar.gz` | Linux tarball |
| `-emit-macos` | Mach-O | macOS executable |
| `-emit-ios` | `.ipa` | iOS app package |
| `-emit-apk` | `.apk` | Android APK |
| `-emit-win` | `.exe` | Windows PE executable |

#### Cross-Compilation Strategy
1. Generate C code (portable)
2. Use appropriate cross-compiler toolchain
3. Package with platform-specific tools

### 4. C to Displexity Transpiler

#### Transpilation Rules
| C Syntax | Displexity Syntax |
|----------|-------------------|
| `int main()` | `func main()` |
| `printf("...")` | `print "..."` |
| `scanf("%d", &x)` | `input x` |
| `int x = 5;` | `int x = 5` |
| `if (cond) { }` | `if cond { }` |
| `for (i=0; i<n; i++)` | `for i in 0..n { }` |
| `while (cond) { }` | `while cond { }` |
| `#include <stdio.h>` | (implicit) |

### 5. IDE Autocomplete System

#### nvim-cmp Integration
- LSP-like completion for Displexity
- Keyword completion for all supported languages
- Function signature hints
- Floating documentation windows

#### Supported Languages
- Displexity (.dis, .dish, .disll, .tuy)
- C/C++ (.c, .cpp, .h, .hpp)
- Python (.py)
- Java (.java)
- JavaScript/TypeScript (.js, .ts)
- Lua (.lua)
- Rust (.rs)
- Shell scripts (.sh, .bash)
- Batch/PowerShell (.bat, .cmd, .ps1)

## Implementation Plan

### Phase 1: Core Compiler Changes
1. Add TUI bytecode generator (`src/tuigen.h`)
2. Add TUI runtime interpreter
3. Change default `-o` behavior
4. Add new emit mode flags

### Phase 2: Cross-Platform Support
1. Add cross-compilation detection
2. Create platform-specific build scripts
3. Implement package generators (deb, rpm, dmg, apk)

### Phase 3: C Transpiler
1. Create C parser (`src/cparser.h`)
2. Implement C-to-Displexity AST converter
3. Add `-transpile` flag

### Phase 4: IDE Enhancements
1. Add nvim-cmp plugin configuration
2. Create Displexity completion source
3. Add multi-language keyword databases

### Phase 5: Testing & Documentation
1. Create comprehensive test suite
2. Update HTML documentation
3. Rebuild installers

## File Changes

### New Files
- `src/tuigen.h` - TUI bytecode generator
- `src/tuiruntime.h` - TUI interpreter
- `src/cparser.h` - C parser for transpiler
- `src/ctranspile.h` - C to Displexity converter
- `scripts/build_linux.sh` - Linux build script
- `scripts/build_macos.sh` - macOS build script
- `scripts/build_android.sh` - Android build script
- `tests/` - Test suite directory

### Modified Files
- `src/disp.cpp` - Add new emit modes, change default behavior
- `gui_ide/nvim/init.lua` - Add autocomplete configuration
- `displexity.nsi` - Update installer
- `release/docs/html/index.html` - Update documentation

## Dependencies

### Build Tools Required
- GCC/MinGW (Windows native)
- mingw-w64 (Linux cross-compile to Windows)
- Android NDK (Android builds)
- Xcode Command Line Tools (macOS/iOS)
- dpkg-deb (Debian packages)
- rpmbuild (RPM packages)
- create-dmg (macOS DMG)

### IDE Dependencies
- nvim-cmp
- cmp-buffer
- cmp-path
- LuaSnip (snippets)
