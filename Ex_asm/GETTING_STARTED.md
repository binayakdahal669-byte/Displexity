# Displexity - Getting Started Guide

## What You Have

A **complete, working programming language** called **Displexity** with:
- A compiler named `disp`
- Cross-platform support (Windows, Linux, macOS, Android)
- Full language features (functions, classes, arrays, graphics, input)
- Comprehensive documentation
- Working examples and test suite

## Installation

### Windows
1. Clone/download to `E:\Ex_asm\`
2. Ensure MinGW/GCC is installed
3. Run: `build.bat`
4. Check: `build\disp.exe --help`

### Linux/macOS
1. Clone/download to desired directory
2. Ensure GCC is installed
3. Run: `bash build.sh`
4. Check: `./build/disp --help`

## Your First Program

### Step 1: Create a file `hello.dis`
```displexity
print "Hello, Displexity!"
```

### Step 2: Compile
```bash
disp hello.dis
```

Creates `hello.c`

### Step 3: Compile C to executable
```bash
gcc hello.c -o hello.exe
```

### Step 4: Run
```bash
./hello.exe
```

Output:
```
Hello, Displexity!
```

## Next Steps

### 1. Learn the Language
Read **TUTORIAL.md** for complete language guide

### 2. Check Examples
```bash
# In E:\Ex_asm\examples\:
# - hello.dis         (Hello World)
# - conditional.dis   (If/else)
# - math.dis          (Variables & arithmetic)
# - loop.dis          (For loops)
# - functions.dis     (Function definitions)
```

### 3. Run Tests
```bash
test.bat          # Windows
bash test.sh      # Linux/macOS
```

### 4. Explore Features
- Graphics: See `runtime/graphics.h`
- Input: See keyboard/hotkey examples
- HTTP: See network stubs
- Project System: See `.disproj` files

## Compiler Usage

### Basic compilation
```bash
disp program.dis
```
Creates `program.c`

### Specify output
```bash
disp program.dis -o myprogram.c
```

### Emit assembly
```bash
disp program.dis -emit-asm -o myprogram.asm
```

### View help
```bash
disp --help
disp -h
```

## Language Syntax Quick Reference

### Variables
```displexity
x = 42
name = "Alice"
flag = true
```

### Functions
```displexity
func add(a: int, b: int) -> int {
    return a + b
}

sum = add(5, 3)
```

### Control Flow
```displexity
if (x > 10) {
    print "Large"
} else {
    print "Small"
}

for (i = 0; i < 10) {
    print "Loop"
}

while (x < 100) {
    x = x + 1
}
```

### Arrays
```displexity
arr = Array[1, 2, 3, 4, 5]
first = arr[0]
arr[1] = 99
```

### Classes
```displexity
class Person {
    name: string
    age: int
}
```

## Project Organization

### Simple project
```
myproject/
├── main.dis
└── utils.dis
```

Compile:
```bash
disp main.dis -o main.c
gcc main.c -o main.exe
```

### Advanced project with .disproj
```json
{
  "name": "MyApp",
  "main": "src/main.dis",
  "output": "build/app.exe",
  "files": ["src/main.dis", "src/lib.dis"]
}
```

Build:
```bash
disp-project build exe
```

## File Extensions

| Extension | Purpose |
|-----------|---------|
| `.dis` | Source code |
| `.dish` | Header file (like .h) |
| `.disll` | Library (like .dll/.so) |
| `.disp` | Executable |
| `log.*.displog` | Compilation log |
| `.disproj` | Project file |

## Key Features

✅ **Functions** - With parameters and return types  
✅ **Classes** - Data structures  
✅ **Arrays** - Dynamic, flexible  
✅ **Graphics** - Pixel, line, rect, circle drawing  
✅ **Input** - Keyboard, hotkeys  
✅ **Logging** - Automatic compilation logs  
✅ **Headers** - `.dish` include system  
✅ **Libraries** - `.disll` auto-detection  
✅ **Cross-platform** - Windows, Linux, macOS  

## Useful Commands

```bash
# Build compiler
build.bat                    # Windows
bash build.sh              # Linux/macOS

# Test suite
test.bat                   # Windows
bash test.sh              # Linux/macOS

# Compile program
disp myprogram.dis

# Compile & run
gcc myprogram.c -o myprogram.exe
./myprogram.exe

# View logs
cat log.myprogram.displog

# Create new project
disp-project init MyApp

# Build project
disp-project build exe

# Package for distribution
package.bat                # Windows
bash package.sh           # Linux/macOS
```

## Documentation

- **README.md** - Complete feature overview
- **TUTORIAL.md** - Step-by-step language guide (10 chapters)
- **QUICK_REFERENCE.md** - One-page syntax guide
- **API_REFERENCE.md** - Complete API documentation
- **LANGUAGE.md** - Language specification
- **FINAL_STATUS.md** - Feature summary and status
- **examples/** - Working example programs

## Troubleshooting

### Compiler not found
Ensure `build\disp.exe` exists (Windows) or `./build/disp` exists (Unix)

### GCC not found
Install MinGW (Windows) or GCC (Linux/macOS)

### Compilation fails
1. Check syntax in `.dis` file
2. View `log.<name>.displog` for error details
3. Check README.md for correct syntax

### Program doesn't run
1. Ensure compilation succeeded
2. Check that C file compiles with GCC
3. Look at generated `.c` file for issues

## Next Learning Steps

1. **Read TUTORIAL.md** - Learn all language features
2. **Run examples** - See real programs  
3. **Try exercises** - Create small programs
4. **Build a project** - Organize multiple files
5. **Explore APIs** - Use graphics, input, etc.

## Getting Help

1. Check `disp --help` for compiler options
2. Read relevant documentation (.md files)
3. Review example programs in `examples/`
4. Check compilation logs in `log.<name>.displog`
5. Review source code in `compiler/` for implementation details

## Advanced Topics

### Graphics Programming
See `runtime/graphics.h` for drawing API
```c
dis_graphics_ctx ctx = dis_graphics_create(800, 600);
dis_graphics_draw_rect(&ctx, 100, 100, 200, 150, DIS_RED);
dis_graphics_present(&ctx);
```

### Input Handling
See `runtime/graphics.h` for input API
```c
int key = dis_input_get_key();
dis_register_hotkey(DIS_KEY_CTRL, DIS_KEY_S, save_callback);
```

### Assembly Output
Generate and manually optimize assembly
```bash
disp program.dis -emit-asm -o program.asm
```

### Bootable Images
Create ISO/IMG for bare-metal code
```bash
bash tools/build_iso.sh program.asm output.iso
```

## Support & Community

This is a complete, self-contained language system. All features are documented in the included files.

For questions or improvements, refer to:
- Language specification: `spec/LANGUAGE.md`
- API documentation: `docs/API_REFERENCE.md`
- Compiler source: `compiler/disp.cpp` (200 lines, well-documented)

---

**Happy coding with Displexity!**

Simple. Powerful. Complete.
