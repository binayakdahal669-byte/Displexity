# Displexity - Getting Started Guide

Welcome to Displexity, a modern programming language designed for terminal games, graphics, and high-performance computing.

## Installation

### Windows
Download and run `displexity-installer.exe` from the [releases page](https://github.com/displexity/displexity/releases).

### Linux / macOS
```bash
git clone https://github.com/displexity/displexity
cd displexity
./build.sh
sudo make install
```

## Your First Program

Create a file named `hello.dis`:

```disp
function main() {
    Array<String> message = ["Hello", "World"];
    print(message[0] + " " + message[1]);
    return 0;
}
```

Compile it:
```bash
disp hello.dis -o hello.exe
```

Run it:
```bash
./hello.exe
```

## Key Concepts

### Variables and Types
- `Int`, `Float`, `String`, `Boolean` (primitives)
- `Array<T>` for dynamic arrays
- `Map<K,V>` for hash maps
- Automatic type inference

### Control Flow
```disp
if (condition) {
    // ...
} else if (other) {
    // ...
} else {
    // ...
}

for (i = 0; i < 10; i = i + 1) {
    // ...
}

while (condition) {
    // ...
}
```

### Functions
```disp
function add(Int a, Int b) -> Int {
    return a + b;
}

Int result = add(5, 3);
```

### I/O
```disp
print("Hello");  // stdout
printErr("Error"); // stderr
String input = readLine();  // stdin
```

## Building Terminal Games

Displexity includes a Terminal User Interface (TUI) library:

```disp
use tui;

function main() {
    TUI::init();
    TUI::clear();
    TUI::drawText(10, 5, "Press Q to quit");
    TUI::refresh();
    
    String key = TUI::getKey();
    TUI::cleanup();
    return 0;
}
```

## Graphics & 3D

For 3D terminal rendering:

```disp
use graphics;

function main() {
    Graphics::init3D();
    
    // Draw a simple cube
    Graphics::drawCube(0, 0, 0, 1.0);
    
    Graphics::render();
    return 0;
}
```

## Working with Libraries

Install a library:
```bash
disp install math
```

Use it in your code:
```disp
use math;

function main() {
    Float result = Math::sin(Math::PI / 2.0);
    print(result);
    return 0;
}
```

## Next Steps

- Read the [Language Guide](LANGUAGE_GUIDE.md) for comprehensive syntax
- Check [API Reference](API_REFERENCE.md) for built-in functions
- Explore [Standard Library](STDLIB.md) documentation
- Build your first [terminal game](GAME_DEV.md)

## Troubleshooting

### Compiler not found
Make sure Displexity is in your PATH. Run:
```bash
echo $PATH  # macOS/Linux
echo %PATH%  # Windows
```

### Import errors
Libraries must be installed. Run:
```bash
disp list  # Show installed libraries
disp install <name>  # Install a library
```

### Build errors
Check the compilation log:
```bash
cat log.myfile.displog  # View details
```

## Getting Help

- [Documentation](https://displexity.github.io)
- [GitHub Issues](https://github.com/displexity/displexity/issues)
- [Discussions](https://github.com/displexity/displexity/discussions)
- Email: hello@displexity.com
