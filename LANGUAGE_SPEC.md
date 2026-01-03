# Displexity Language Specification v0.1

## Overview
Displexity is a simple, compiled programming language designed for ease of use, multiplatform support, and direct hardware access through assembly.

## File Extensions
- `.dis` - Displexity source file
- `.dish` - Displexity header file (like C headers)
- `.disll` - Displexity library file (like .dll/.so)
- `.disp` - Displexity executable

## Basic Syntax

### Variables
```
x = 10;
name = "Hello";
flag = true;
```

### Data Types
- `int` - Integer (32-bit)
- `string` - Text
- `bool` - Boolean (true/false)
- `Array` - Dynamic array

### Arrays
```
// Array literals
arr = Array[1, 2, 3, 4];

// Array access
value = arr[0];  // Gets first element

// Array assignment
arr[1] = 99;
```

### Functions
```
func add(a: int, b: int) -> int {
    result = a + b;
    return result;
}

// Function call
value = add(10, 20);
```

### Classes
```
class Person {
    name: string;
    age: int;
}
```

### Control Flow
```
// If/else
if (x > 10) {
    print "Large";
} else {
    print "Small";
}

// While loop
while (x < 100) {
    x = x + 1;
}

// For loop
for (i = 0; i < 10) {
    print "Iteration";
}
```

### I/O
```
print "Hello World";
```

### Headers
```
include "mylib.dish";
```

## Compilation Pipeline
```
.dis source -> Lexer -> Parser -> AST -> Code Generator -> C/ASM -> Executable
```

## Compiler Output
- Logs stored as `log.<basename>.displog`
- Default C output for cross-platform
- ASM output for direct hardware access

## Project Files (.disproj)
```json
{
  "name": "MyProject",
  "main": "src/main.dis",
  "output": "build/output.disp",
  "target": "exe",
  "files": [
    "src/main.dis",
    "src/lib.dis"
  ],
  "libraries": [
    "std.disll"
  ]
}
```

## Features
- Functions with type annotations
- Dynamic arrays with zero-copy access
- Class definitions (basic)
- HTTP requests (stdlib)
- Graphics API (abstracted, no OpenGL)
- Direct ASM compilation for hardware control
- No HWND, HINSTANCE, or low-level Win32 nonsense
