# Displexity Quick Reference Card

## Syntax

### Variables
```displexity
x = 42                  // integer
name = "Alice"          // string  
flag = true             // boolean
```

### Arrays
```displexity
arr = Array[1, 2, 3]    // create array
first = arr[0]          // read element
arr[1] = 99             // write element
```

### Operators
```
+  -  *  /  %           // arithmetic
<  >  <=  >=  ==  !=    // comparison
&&  ||  !               // logical
=  += -= *= /=          // assignment
```

### Control Flow
```displexity
if (condition) { }
if (condition) { } else { }

while (condition) { }

for (i = 0; i < 10) { }
```

### Functions
```displexity
func name(a: int, b: string) -> int {
    return a
}
```

### Classes
```displexity
class Person {
    name: string
    age: int
}
```

### I/O
```displexity
print "Hello"
```

### Comments
```displexity
// This is a comment
```

## Compilation

### Build Compiler
```bash
.\build.bat              # Windows
bash build.sh            # Linux/macOS
```

### Compile Program
```bash
disp program.dis         # -> program.c
disp program.dis -o out.c -emit-c
disp program.dis -o out.asm -emit-asm
```

### Run Program
```bash
gcc program.c -o program.exe
./program.exe
```

## Files

| Type | Purpose |
|------|---------|
| `.dis` | Source code |
| `.dish` | Header/module |
| `.disll` | Library |
| `.disp` | Executable |
| `log.<name>.displog` | Compilation log |
| `.disproj` | Project file |

## CLI Options

```
disp <file.dis>           # Compile to C
disp <file.dis> -o <out>  # Specify output
disp <file.dis> -emit-asm # Emit assembly
disp -h                   # Show help
disp --help               # Show full help
```

## Type System

| Type | Example | Size |
|------|---------|------|
| `int` | `42` | 32-bit |
| `string` | `"hello"` | Dynamic |
| `bool` | `true` | 1 byte |
| `Array` | `[1, 2, 3]` | Dynamic |

## Common Patterns

### Loop Counter
```displexity
for (i = 0; i < 10) {
    print "Count"
}
```

### Array Iteration
```displexity
arr = Array[10, 20, 30]
for (i = 0; i < 3) {
    val = arr[i]
}
```

### Function Definition
```displexity
func double(x: int) -> int {
    return x * 2
}
```

### Conditional Logic
```displexity
if (x > 10) {
    print "Large"
} else {
    print "Small"
}
```

### Array Operation
```displexity
sum = 0
for (i = 0; i < 5) {
    sum = sum + arr[i]
}
```

## Hello World

**File: hello.dis**
```displexity
print "Hello, World!"
```

**Compile & Run:**
```bash
disp hello.dis
gcc hello.c -o hello.exe
./hello.exe
```

**Output:**
```
Hello, World!
```

## Project Template

```
myproject/
├── src/
│   ├── main.dis
│   └── lib.dis
├── build/
│   └── (compiled files)
├── project.disproj
└── README.md
```

## Compilation Pipeline

```
program.dis
    ↓
[Lexer]      → tokens
    ↓
[Parser]     → AST
    ↓
[CodeGen]    → C code
    ↓
[GCC]        → machine code
    ↓
program.exe
```

## Performance Tips

1. Use type annotations for clarity
2. Avoid deeply nested loops
3. Pre-calculate values outside loops
4. Use functions for code reuse
5. Check array bounds manually

## Error Handling

- Check `log.<name>.displog` for compilation details
- Look for error messages in terminal output
- Verify input file exists
- Ensure GCC is installed
- Check for syntax errors (quotes, parentheses)

## Resources

- **README.md** - Full documentation
- **TUTORIAL.md** - Language tutorial
- **LANGUAGE.md** - Language specification
- **examples/** - Working example programs
- **COMPLETE.md** - Feature summary

---

For more help: `disp --help`
