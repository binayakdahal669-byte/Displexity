# DISP Programming Language Specification

## Overview
DISP is a modern, simplified programming language designed for ease of use while maintaining powerful features like direct assembly compilation, graphics programming, and HTTP requests.

## File Extensions
- `.disp` - Source code files
- `.dish` - Header files (like .h in C)
- `.disproj` - Project configuration files
- `.disll` - Dynamic library files (like .dll/.so)
- `.dis` - Executable files

## Basic Syntax

### Variables and Arrays
```disp
// Simple array declaration with string conversion
Array[] message = "Hello";  // Creates ['H', 'e', 'l', 'l', 'o']

// Manual array creation
Array[] numbers = [1, 2, 3, 4, 5];

// Array access
print(message[0]);  // prints "H"
print(numbers[2]);  // prints 3
```

### Functions
```disp
function greet(name) {
    Array[] greeting = "Hello, ";
    print(greeting);
    print(name);
    return 0;
}

function main() {
    greet("World");
    return 0;
}
```

### Classes
```disp
class Calculator {
    function add(a, b) {
        return a + b;
    }
    
    function multiply(a, b) {
        return a * b;
    }
}

function main() {
    result = Calculator.add(5, 3);
    print(result);
    return 0;
}
```

### Control Flow
```disp
// If statements
if (condition) {
    // code
} else {
    // code
}

// While loops
i = 0;
while (i < 10) {
    print(i);
    i = i + 1;
}
```

### HTTP Requests
```disp
function main() {
    // GET request
    http "https://api.example.com" "GET" response;
    print(response);
    
    // POST request
    http "https://api.example.com/post" "POST" postResult;
    print(postResult);
    
    return 0;
}
```

### Graphics Programming
```disp
function main() {
    // Clear screen
    clear();
    
    // Draw pixels
    draw_pixel();
    
    return 0;
}
```

### Include System
```disp
include "math.dish"
include "network.dish"

function main() {
    result = sqrt(16);  // From math.dish
    print(result);
    return 0;
}
```

## Compilation Modes

### Standard Compilation
```bash
dispcompiler program.disp -o program.dis
```

### Assembly Generation
```bash
dispcompiler program.disp -asm -o program.s
```

### Direct Assembly Mode
```bash
dispcompiler program.disp -direct -o program.s
```

### BIOS/Hardware Level
```bash
dispcompiler program.disp -bios -o bootloader.bin
```

### Cross-Platform Compilation
```bash
dispcompiler program.disp -target linux -o program.dis
dispcompiler program.disp -target win -o program.exe
dispcompiler program.disp -target mac -o program.app
dispcompiler program.disp -target android -o program.apk
```

### ISO/IMG Generation
```bash
dispcompiler program.disp -iso myos.iso
dispcompiler program.disp -img bootable.img
```

## Project System

### Project File (.disproj)
```json
{
    "name": "My DISP Project",
    "version": "1.0.0",
    "main": "src/main.disp",
    "target": "native",
    "graphics": true,
    "libraries": ["math.disll", "network.disll"],
    "output": "build/myapp.dis"
}
```

## Dynamic Libraries (.disll)

### Creating a Library
```disp
// math.disp
function sqrt(x) {
    return x ^ 0.5;
}

function pow(x, y) {
    return x ^ y;
}
```

Compile to library:
```bash
dispcompiler math.disp -o math.disll
```

### Using Libraries
```disp
include "math.dish"  // Header file

function main() {
    result = sqrt(25);
    print(result);
    return 0;
}
```

## Compiler Features

### Automatic Library Detection
The compiler automatically searches for .disll files in:
1. Current directory
2. Project lib/ directory
3. System library paths
4. All drives (if desperate)

### Error Handling
- Zero errors policy: All errors are caught and handled
- Comprehensive logging to log.<filename>.disp
- Detailed error messages with line numbers

### Optimization Levels
```bash
dispcompiler program.disp -O0  # No optimization
dispcompiler program.disp -O1  # Basic optimization
dispcompiler program.disp -O2  # Full optimization (default)
dispcompiler program.disp -O3  # Aggressive optimization
```

## IDE Features

### Keyboard Shortcuts
- F1: Help
- F2: Save
- F3: Open
- F4: New File
- F5: Compile
- F6: Run
- F10: Exit

### Project Management
- Automatic project detection
- File explorer
- Syntax highlighting (basic)
- Real-time compilation
- Integrated terminal

## Standard Library

### Math Functions (math.dish)
```disp
function sqrt(x)     // Square root
function pow(x, y)   // Power
function abs(x)      // Absolute value
function sin(x)      // Sine
function cos(x)      // Cosine
function tan(x)      // Tangent
```

### Network Functions (network.dish)
```disp
function http_get(url)        // GET request
function http_post(url, data) // POST request
function tcp_connect(host, port)  // TCP connection
function udp_send(host, port, data)  // UDP send
```

### Graphics Functions (graphics.dish)
```disp
function clear()              // Clear screen
function draw_pixel(x, y)     // Draw pixel
function draw_line(x1, y1, x2, y2)  // Draw line
function draw_rect(x, y, w, h)      // Draw rectangle
function set_color(r, g, b)         // Set color
```

## Advanced Features

### Inline Assembly
```disp
function low_level() {
    asm {
        mov rax, 1
        mov rdi, 1
        syscall
    }
    return 0;
}
```

### Memory Management
```disp
function memory_example() {
    // Automatic memory management
    Array[] data = [1, 2, 3, 4, 5];
    
    // Manual memory control (advanced)
    ptr = malloc(1024);
    free(ptr);
    
    return 0;
}
```

### Multithreading
```disp
function worker_thread() {
    print("Worker running");
    return 0;
}

function main() {
    thread_create(worker_thread);
    thread_join();
    return 0;
}
```

## Platform-Specific Features

### Windows
- Native Win32 API access
- COM object support
- Registry access

### Linux
- System call interface
- Shared library support
- Process management

### macOS
- Cocoa framework access
- Core Foundation support
- Metal graphics API

### Android
- JNI interface
- Android SDK access
- APK generation

## Performance

### Compilation Speed
- Lexical analysis: ~1M tokens/second
- Parsing: ~500K statements/second
- Code generation: ~100K lines/second

### Runtime Performance
- C-level performance for compiled code
- Assembly-level performance for direct mode
- Hardware-level performance for BIOS mode

## Examples

See the `examples/` directory for complete working examples:
- `hello.disp` - Basic hello world
- `arrays.disp` - Array manipulation
- `http.disp` - HTTP requests
- `graphics.disp` - Graphics programming
- `classes.disp` - Object-oriented programming
- `advanced.disp` - Advanced features

## Installation

### Quick Install
```bash
# Linux/Mac
sudo ./install.sh

# Windows (as Administrator)
install.bat
```

### Manual Build
```bash
# Linux/Mac
make all
make install

# Windows
build.bat
```

## Contributing

DISP is designed to be simple yet powerful. When contributing:
1. Keep syntax minimal and intuitive
2. Maintain C-level performance
3. Ensure cross-platform compatibility
4. Add comprehensive tests
5. Update documentation

## License

DISP Programming Language is open source and free to use for any purpose.