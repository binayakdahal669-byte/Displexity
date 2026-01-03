# Displexity Programming Language

A complete, modern programming language written in C++ with compiler, runtime, and IDE support. Compiles to C, assembly, and executables with zero external dependencies (no CMake, no Rust, no TOML).

## Quick Start

### Build the Compiler

**Windows:**
```cmd
build.bat
```

**Linux/macOS:**
```bash
bash build.sh
```

### Compile a Program

```bash
./disp examples/hello.dis -o build/hello.c
gcc build/hello.c -o build/hello
./build/hello
```

Output: `Hello, Displexity!`

## Features

✅ **Complete language** with functions, classes, arrays, loops, conditionals  
✅ **Compiler** (`disp`) that emits C or x86-64 ASM  
✅ **Runtime library** with arrays, HTTP stubs, graphics API  
✅ **IDE** skeleton with syntax highlighting and build integration  
✅ **Cross-platform** - builds on Windows, Linux, macOS, Android  
✅ **No external deps** - pure C++, no CMake/Rust/TOML  
✅ **Logging** - automatic `log.<name>.displog` for compilation details  
✅ **.disproj** project files for multi-file builds  
✅ **Headers** `.dish` for modular code  
✅ **Libraries** `.disll` auto-detected by compiler  

## Language Syntax

### Variables & Types
```displexity
x = 10;           // int
name = "Alice";   // string
flag = true;      // bool
```

### Arrays
```displexity
arr = Array[1, 2, 3];
first = arr[0];       // Access
arr[1] = 99;          // Assign
```

### Functions
```displexity
func add(a: int, b: int) -> int {
    return a + b;
}

sum = add(5, 3);
```

### Control Flow
```displexity
if (x > 10) {
    print "Large";
} else {
    print "Small";
}

while (x < 100) {
    x = x + 1;
}

for (i = 0; i < 10) {
    print "Loop";
}
```

### Classes
```displexity
class Person {
    name: string;
    age: int;
}
```

### I/O
```displexity
print "Hello World";
```

## File Structure

```
e:\Ex_asm\
├── compiler/
│   ├── disp.cpp          # Main compiler driver
│   ├── lexer.h           # Tokenizer
│   ├── parser.h          # Syntax parser
│   ├── ast.h             # Abstract syntax tree
│   └── codegen.h         # C/ASM code generator
├── runtime/
│   └── runtime.h         # Standard library
├── ide/
│   └── main.cpp          # IDE skeleton
├── examples/
│   ├── hello.dis         # Hello World
│   ├── math.dis          # Arithmetic
│   ├── loop.dis          # Loops
│   ├── conditional.dis   # If/else
│   └── functions.dis     # Functions
├── spec/
│   └── LANGUAGE.md       # Full language spec
├── build.sh / build.bat  # Build scripts
└── README.md             # This file
```

## Compiler Usage

```
disp <input.dis> [options]

Options:
  -o <file>        Output file (default: input.c)
  -emit-c          Emit C source (default)
  -emit-asm        Emit x86-64 assembly
  -h, --help       Show help

Output:
  - Compiled code in specified format
  - Automatic log file: log.<basename>.displog
```

## Example: Compiling to Different Formats

**Emit C:**
```bash
disp program.dis -o program.c -emit-c
gcc program.c -o program.exe
./program.exe
```

**Emit ASM (x86-64):**
```bash
disp program.dis -o program.asm -emit-asm
# Further assembly with system assembler
```

**Direct to Executable:**
```bash
disp program.dis -o program.exe
./program.exe
```

## Project Files (.disproj)

Create a `project.disproj` to organize multi-file projects:

```json
{
  "name": "MyApp",
  "main": "src/main.dis",
  "output": "build/app.disp",
  "target": "exe",
  "files": [
    "src/main.dis",
    "src/lib.dis",
    "src/utils.dis"
  ],
  "libraries": [
    "std.disll"
  ]
}
```

## Headers & Libraries

**Include headers:**
```displexity
include "mylib.dish";
```

**Automatic library detection:**
The compiler searches for `.disll` files in:
- Current directory
- `lib/` subdirectory
- Each drive (Windows)

## Logging

Every compilation produces a log file:

```
log.hello.displog
log.math.displog
log.conditional.displog
```

Contents:
```
=== Displexity Compilation Log ===
Input: examples/hello.dis
Output: build/hello.c
Mode: c
Tokens: 12
Status: Success
```

## Extensions

| Extension | Purpose |
|-----------|---------|
| `.dis`    | Source code |
| `.dish`   | Header file |
| `.disll`  | Library (like .dll/.so) |
| `.disp`   | Executable |
| `log.*.displog` | Compilation log |

## Runtime Library

The generated C code includes stubs for:

- **Arrays** - Dynamic, zero-copy
- **Strings** - Managed memory
- **HTTP** - Network requests (stub)
- **Graphics** - Abstracted API (no OpenGL/Win32)
- **Math** - Basic operations

## IDE

**displexity-ide** (stub included):
- Opens `.dis` and `.dish` files
- Syntax highlighting
- Integrated compiler
- Error/warning display
- Log viewer
- Cross-platform (Windows, Linux, macOS, APK)

## Testing

Run all example tests:

**Windows:**
```cmd
build.bat test
```

**Linux/macOS:**
```bash
bash build.sh test
```

Tests compile and run:
- `hello.dis` - Basic output
- `math.dis` - Arithmetic
- `conditional.dis` - If/else logic

## Packaging

Create redistributable binaries:

**Windows:**
```cmd
package.bat
```

**Linux/macOS:**
```bash
bash package.sh
```

Creates:
- `dist/displexity-0.1.0-windows.zip`
- `dist/displexity-0.1.0-linux.tar.gz`
- `dist/displexity-0.1.0-macos.tar.gz`
- `dist/displexity-0.1.0-android.apk` (placeholder)

## Design Principles

1. **No external dependencies** - Pure C++17, uses system C compiler
2. **Simple syntax** - Easy to learn, Python-like readability
3. **Efficient output** - Generates fast C or direct assembly
4. **Multi-target** - Compile once, run anywhere
5. **Zero boilerplate** - No HWND, HINSTANCE, Win32 nonsense
6. **Direct hardware access** - Assembly output for low-level code
7. **Modular design** - Headers, libraries, and project files

## Future Enhancements

- [ ] Direct x86-64 ASM backend (no C intermediate)
- [ ] ISO/IMG image builder (bootable disks)
- [ ] OpenGL graphics API abstraction
- [ ] Full AI/ML standard library
- [ ] Network stack (sockets, DNS)
- [ ] File I/O operations
- [ ] Error handling with exceptions
- [ ] Generics/templates
- [ ] Module system improvements
- [ ] REPL for interactive development

## License

MIT - See LICENSE file

## Getting Help

1. Check `spec/LANGUAGE.md` for language features
2. Review `examples/` for sample programs
3. Check `log.<name>.displog` files for compilation details
4. Run `disp -h` for compiler options

