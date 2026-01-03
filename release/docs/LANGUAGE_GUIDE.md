# Displexity Language Guide

Complete syntax reference and programming guide for the Displexity language.

## Table of Contents

1. [Basic Syntax](#basic-syntax)
2. [Variables and Types](#variables-and-types)
3. [Control Flow](#control-flow)
4. [Functions](#functions)
5. [Arrays and Strings](#arrays-and-strings)
6. [Graphics Programming](#graphics-programming)
7. [File I/O and Libraries](#file-io-and-libraries)
8. [Advanced Features](#advanced-features)

## Basic Syntax

### Comments
```dis
// Single line comment

/* Multi-line
   comment */
```

### Output
```dis
print("Hello World")           # Print with newline
print("Value:", x)             # Multiple arguments
print(f"Name: {name}")         # F-string formatting
```

### Input
```dis
name = input("Name? ")         # String input
age = int(input("Age? "))      # Integer input
height = float(input("Height? "))  # Float input
```

## Variables and Types

### Type System
Displexity supports both explicit and implicit typing:

```dis
// Explicit types
num x = 42                  // Integer
real y = 3.14              // Float
text name = "Alice"        // String
flag active = yes          // Boolean
var data = "anything"      // Universal type

// Type aliases (all equivalent)
int x = 42                 // Same as num
float y = 3.14            // Same as real
str name = "Alice"        // Same as text
bool active = true        // Same as flag
```

### Variable Declaration
```dis
# Python-style variable assignment
score = 100
player = "Player1"
health = 75.5
active = True

# Type hints (optional)
score: int = 100
name: str = "Alice"
height: float = 5.8
ready: bool = False
```

### Type Conversion
```dis
num x = 42
real y = float << x        // Convert to float
text s = str << x          // Convert to string
num z = int << y           // Convert to int
```

## Control Flow

### Conditionals
```dis
# Basic if-else
if score > 100:
    print("High score!")
else:
    print("Try again")

# Multiple conditions
if health <= 0:
    print("Game Over")
elif health < 20:
    print("Low health!")
else:
    print("Healthy")
```

### Loops
```dis
# For loop with range
for i in range(10):
    print(i)

# For loop with step
for i in range(0, 100, 5):
    print(i)

# While loop
while health > 0:
    # Game logic
    health -= 1

# List iteration
numbers = [1, 2, 3, 4, 5]
for num in numbers:
    print(num)
```

### Break and Continue
```dis
repeat i from 0 to 100 {
    when (i == 50) break      // Exit loop
    when (i % 2 == 0) next    // Skip to next iteration
    show i
}
```

## Functions

### Function Definition
```dis
// Basic function
def greet(text name) {
    say "Hello " << name
}

// Function with return value
def add(num a, num b) {
    give a + b              // 'give' is same as 'return'
}

// Multiple parameter styles
def calculate(num x, real y: float, text msg) {
    // Mix of C-style and Python-style parameters
}
```

### Function Calls
```dis
greet("Alice")
num result = add(5, 3)
show "Result: " << result
```

### Advanced Functions
```dis
// Recursive function
def factorial(num n) {
    when (n <= 1) give 1
    give n * factorial(n - 1)
}

// Function with default parameters
def power(num base, num exp = 2) {
    num result = 1
    repeat i from 0 to exp {
        result = result * base
    }
    give result
}
```

## Arrays and Strings

### Array Declaration
```dis
// Array literal
num scores[] = [100, 85, 92, 78]

// String as character array
text message[] = "Hello"
// Equivalent to: ['H', 'e', 'l', 'l', 'o', '\0']

// Empty array
num data[10]               // 10 integers, initialized to 0
```

### Array Access
```dis
num first = scores[0]      // Get first element
scores[1] = 95            // Set second element

// String character access
text name = "Alice"
text first_char = name[0]  // 'A'
name[0] = 'B'             // Changes to "Blice"
```

### Array Operations
```dis
// Array length (conceptual - not built-in yet)
num length = sizeof(scores) / sizeof(scores[0])

// Array iteration
repeat i from 0 to length {
    show scores[i] << " "
}
```

## Graphics Programming

### Window Creation
```dis
win "My Game", 800, 600    // Create window
window("Title", 1024, 768) // Alternative syntax
```

### Graphics Initialization
```dis
opengl()                   // Initialize OpenGL
2d()                      // Set 2D projection
3d(60.0, 0.1, 100.0)     // Set 3D projection (FOV, near, far)
```

### 2D Graphics
```dis
cls()                     // Clear screen
color 1.0, 0.0, 0.0      // Set red color
box 100, 100, 50, 50     // Draw rectangle
circle 200, 200, 30      // Draw circle
line 0, 0, 100, 100      // Draw line
pixel 150, 150           // Draw single pixel
flip()                   // Swap buffers (display)
```

### 3D Graphics
```dis
3d(60.0, 0.1, 100.0)     // Set 3D projection

push()                   // Save transformation matrix
move 0, 0, -5           // Translate
turn 45, 1, 0, 0        // Rotate 45° around X axis
size 2, 2, 2            // Scale 2x

// Draw 3D objects
box -1, -1, -1, 2, 2, 2  // 3D cube
triangle 0, 1, 0, -1, -1, 0, 1, -1, 0  // 3D triangle

pop()                    // Restore transformation matrix
```

### Colors and Materials
```dis
color 1.0, 0.5, 0.2      // RGB color
bg_color 0.1, 0.1, 0.2, 1.0  // Background color with alpha
```

## File I/O and Libraries

### Including Files
```dis
include "math_lib.dis"    // Include source file
import "graphics.dish"    // Include header file
use "physics.disll"       // Include binary library
```

### Multi-file Projects
```dis
// main.dis
include "player.dis"
include "enemies.dis"

def main() {
    init_player()
    spawn_enemies()
    game_loop()
}

// player.dis
def init_player() {
    // Player initialization
}

// enemies.dis  
def spawn_enemies() {
    // Enemy spawning logic
}
```

### Creating Libraries
```dis
// Compile to binary library
// Command: disp math_lib.dis -emit-lib -o math.disll

def add(num a, num b) {
    give a + b
}

def multiply(num a, num b) {
    give a * b
}
```

## Advanced Features

### HTTP Requests
```dis
text response = web("https://api.example.com/data")
show "Response: " << response
```

### Input Handling
```dis
num key = keypress()      // Get key press
text key_str = key()      // Get key as string

// Hotkey registration
hotkey('q', quit_game)    // Register Q key to call quit_game()
```

### AI Integration
```dis
text response = ai("What is the meaning of life?")
show "AI says: " << response
```

### Memory Management
```dis
// Arrays are automatically managed
num big_array[1000000]    // Automatically allocated

// Strings are automatically managed
text long_string = "Very long string..."
```

### Error Handling
```dis
// Basic error checking
when (file_exists("data.txt")) {
    // Process file
} otherwise {
    say "File not found!"
}
```

### Compilation Targets

#### Native Executable
```bash
disp game.dis -o game.exe
```

#### WebAssembly
```bash
disp game.dis -emit-wasm -o game.wat
wat2wasm game.wat -o game.wasm
```

#### Bare Metal Assembly
```bash
disp bootloader.dis -bare-asm -o boot.s
nasm -f bin boot.s -o boot.bin
```

#### C Source Code
```bash
disp program.dis -emit-c -o program.c
gcc program.c -o program
```

## Best Practices

### Code Organization
```dis
// Use meaningful names
def calculate_damage(num attack, num defense) {
    give attack - defense / 2
}

// Group related functionality
def init_graphics() {
    win "Game", 800, 600
    opengl()
    2d()
}
```

### Performance Tips
```dis
// Prefer local variables in loops
repeat i from 0 to 1000 {
    num local_var = expensive_calculation()  // Good
    // global_var = expensive_calculation()  // Slower
}

// Use appropriate types
num counter = 0           // For integers
real position = 0.0      // For floating point
flag is_active = yes     // For booleans
```

### Debugging
```dis
// Use descriptive output
show "Player position: " << x << ", " << y
show "Health: " << health << "/" << max_health

// Add debug flags
flag debug = yes
when (debug) {
    say "Debug: entering function"
}
```

## Common Patterns

### Game Loop
```dis
win "Game", 800, 600
opengl()
2d()

repeat frame from 0 to 10000 {
    // Input
    handle_input()
    
    // Update
    update_game_state()
    
    // Render
    cls()
    draw_game()
    flip()
}
```

### State Machine
```dis
num state = 0  // 0=menu, 1=playing, 2=paused

repeat frame from 0 to 10000 {
    when (state == 0) {
        draw_menu()
        when (key_pressed('s')) state = 1
    } otherwise when (state == 1) {
        update_game()
        draw_game()
        when (key_pressed('p')) state = 2
    } otherwise when (state == 2) {
        draw_pause_menu()
        when (key_pressed('p')) state = 1
    }
    flip()
}
```

### Physics Simulation
```dis
real x = 0, y = 0          // Position
real vx = 5, vy = 0        // Velocity
real gravity = -9.8        // Acceleration

repeat frame from 0 to 1000 {
    // Update physics
    vy = vy + gravity * 0.016  // 60 FPS
    x = x + vx * 0.016
    y = y + vy * 0.016
    
    // Collision with ground
    when (y < 0) {
        y = 0
        vy = -vy * 0.8  // Bounce with damping
    }
    
    // Render
    cls()
    circle x, y, 10
    flip()
}
```

This guide covers the essential features of Displexity. For more examples, see the `examples/` directory and the API reference documentation.