# Displexity Complete API Reference

## Core Language Features

### 1. Variables & Types
```displexity
// Integer
x = 42;
y = -100;

// String
name = "Alice";
message = "Hello, World!";

// Boolean
flag = true;
done = false;

// Arrays
arr = Array[1, 2, 3, 4, 5];
single = arr[0];        // Get element
arr[1] = 99;            // Set element
```

### 2. Functions
```displexity
// Basic function
func add(a: int, b: int) -> int {
    result = a + b;
    return result;
}

// No parameters
func hello() {
    print "Hello!";
}

// Call function
sum = add(5, 3);
hello();
```

### 3. Classes
```displexity
class Person {
    name: string;
    age: int;
}

class Rectangle {
    width: int;
    height: int;
}
```

### 4. Control Flow
```displexity
// If/else
if (x > 10) {
    print "Large";
} else if (x > 5) {
    print "Medium";
} else {
    print "Small";
}

// While loop
while (count < 10) {
    print "Count";
    count = count + 1;
}

// For loop
for (i = 0; i < 10) {
    print "Loop";
}
```

### 5. Operators
```displexity
// Arithmetic
a = 5 + 3;      // 8
b = 10 - 2;     // 8
c = 4 * 5;      // 20
d = 20 / 4;     // 5
e = 17 % 5;     // 2

// Comparison
x == y          // Equal
x != y          // Not equal
x > y           // Greater than
x < y           // Less than
x >= y          // Greater than or equal
x <= y          // Less than or equal

// Logical
a && b          // AND
a || b          // OR
!a              // NOT
```

## Runtime Library

### Input/Output

#### Print
```displexity
print "Hello, World!";
print 42;
print flag;
```

### Graphics API

#### Create graphics context
```c
dis_graphics_ctx ctx = dis_graphics_create(800, 600);
```

#### Draw operations
```c
// Clear screen
dis_graphics_clear(&ctx, DIS_BLACK);

// Draw pixel
dis_graphics_set_pixel(&ctx, 100, 100, DIS_RED);

// Draw line
dis_graphics_draw_line(&ctx, 0, 0, 100, 100, DIS_GREEN);

// Draw rectangle
dis_graphics_draw_rect(&ctx, 50, 50, 200, 200, DIS_BLUE);

// Draw circle
dis_graphics_draw_circle(&ctx, 400, 300, 50, DIS_WHITE);

// Present/update
dis_graphics_present(&ctx);
```

#### Colors
```c
DIS_RED
DIS_GREEN
DIS_BLUE
DIS_WHITE
DIS_BLACK
DIS_COLOR(r, g, b, a)  // Custom color
```

### Input Handling

#### Key codes
```c
DIS_KEY_A through DIS_KEY_Z
DIS_KEY_0 through DIS_KEY_9
DIS_KEY_SPACE
DIS_KEY_ENTER
DIS_KEY_ESC
DIS_KEY_UP, DIS_KEY_DOWN, DIS_KEY_LEFT, DIS_KEY_RIGHT
DIS_KEY_SHIFT
DIS_KEY_CTRL
```

#### Key input
```c
// Check if key is pressed
int pressed = dis_input_is_key_down(DIS_KEY_A);

// Get next key event
dis_key_event evt = dis_input_poll_event();

// Register hotkey
void my_callback() { /* ... */ }
dis_register_hotkey(DIS_KEY_CTRL, DIS_KEY_S, my_callback);
```

### Array Operations

#### Create array
```c
dis_array arr = dis_array_create(sizeof(int), 10);
```

#### Array access
```c
int value = 42;
dis_array_push(&arr, &value);
int* elem = (int*)dis_array_get(&arr, 0);
dis_array_set(&arr, 0, &value);
```

#### Cleanup
```c
dis_array_free(&arr);
```

### String Operations

#### Create string
```c
dis_string str = dis_string_create("Hello");
```

#### Free string
```c
dis_string_free(&str);
```

### Network/HTTP (Stubs)

#### HTTP GET
```c
dis_http_response resp = dis_http_get("http://example.com");
// resp.response contains result
dis_http_response_free(&resp);
```

## Headers & Libraries

### Include headers
```displexity
include "mylib.dish";
include "graphics.dish";
include "utils.dish";
```

### Auto-detected libraries
```displexity
// Compiler automatically links:
// - stdlib.disll
// - graphics.disll
// - network.disll
```

## Compiler Options

### Command-line usage
```bash
disp <input.dis> [options]

Options:
  -o <file>           Output file
  -emit-c             Emit C source (default)
  -emit-asm           Emit x86-64 assembly
  -O0, -O1, -O2, -O3  Optimization level
  -h, --help          Show help
  -v, --version       Show version
```

### Output formats
```bash
# Generate C
disp program.dis -o program.c -emit-c
gcc program.c -o program.exe

# Generate ASM
disp program.dis -o program.asm -emit-asm
nasm -f elf64 program.asm -o program.o

# Direct executable (via C)
disp program.dis -o program.exe
```

## Project Files (.disproj)

### Project structure
```json
{
  "name": "MyProject",
  "version": "1.0.0",
  "main": "src/main.dis",
  "output": "build/app.disp",
  "target": "exe",
  
  "files": [
    "src/main.dis",
    "src/lib.dis"
  ],
  
  "libraries": [
    "stdlib.disll",
    "graphics.disll"
  ],
  
  "targets": {
    "windows": { "arch": "x86-64" },
    "linux": { "arch": "x86-64" },
    "iso": { "bootloader": "boot/loader.asm" }
  }
}
```

### Build targets
```bash
disp-project build exe      # Build executable
disp-project build iso      # Build bootable ISO
disp-project build img      # Build disk image
disp-project build lib      # Build shared library
```

## Multi-language Compilation

### Compile to C
```bash
disp program.dis -emit-c
```

### Compile to ASM
```bash
disp program.dis -emit-asm
```

### Compile to other languages (future)
```bash
disp program.dis -emit-python
disp program.dis -emit-javascript
disp program.dis -emit-rust
```

## Logging

### Automatic log files
Every compilation creates:
```
log.<basename>.displog
```

### Log contents
```
=== Displexity Compilation Log ===
Input: program.dis
Output: program.c
Mode: c
Tokens: 45
Statements: 12
Status: Success
```

## Error Handling

### Compile-time errors
```
Parse error: Unexpected token at line 10
Type error: Variable 'x' not declared
```

### Runtime errors
Logged to `log.<name>.displog` with details

## Performance Notes

- C backend: Fast, portable, GCC/Clang optimizations
- ASM backend: Bare metal, maximum control, no runtime overhead
- Graphics: Framebuffer-based, no dependencies on OpenGL/DirectX
- Arrays: Dynamic, zero-copy access, grows as needed

## Future Enhancements

- [ ] Exception handling (try/catch)
- [ ] Generics/Templates
- [ ] Pattern matching
- [ ] Async/await
- [ ] Module system
- [ ] Type inference
- [ ] Macros
- [ ] Reflection
