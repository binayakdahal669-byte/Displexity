# Displexity API Reference

Complete reference for all built-in functions, keywords, and system features.

## Table of Contents

1. [Core Language](#core-language)
2. [Input/Output](#inputoutput)
3. [Graphics System](#graphics-system)
4. [Math Functions](#math-functions)
5. [String Operations](#string-operations)
6. [System Functions](#system-functions)
7. [Compilation Directives](#compilation-directives)

## Core Language

### Keywords and Aliases

#### Output Keywords
| Keyword | Aliases | Description | Example |
|---------|---------|-------------|---------|
| `print` | `say`, `show` | Print text to console | `say "Hello"` |

#### Input Keywords
| Keyword | Aliases | Description | Example |
|---------|---------|-------------|---------|
| `input` | `ask`, `get`, `read`, `readline` | Get user input | `ask "Name?" -> name` |

#### Control Flow
| Keyword | Aliases | Description | Example |
|---------|---------|-------------|---------|
| `if` | `when` | Conditional statement | `when (x > 0) { ... }` |
| `else` | `otherwise` | Alternative condition | `otherwise { ... }` |
| `while` | `loop` | While loop | `loop while (x > 0) { ... }` |
| `for` | `repeat` | For loop | `repeat i from 0 to 10 { ... }` |

#### Function Definition
| Keyword | Aliases | Description | Example |
|---------|---------|-------------|---------|
| `func` | `function`, `def` | Define function | `def greet(text name) { ... }` |
| `return` | `give` | Return value | `give result` |

#### File Operations
| Keyword | Aliases | Description | Example |
|---------|---------|-------------|---------|
| `include` | `import`, `use` | Include file | `include "math.dis"` |

#### Type Keywords
| Keyword | Aliases | Description | Example |
|---------|---------|-------------|---------|
| `int` | `num`, `number` | Integer type | `num x = 42` |
| `float` | `real`, `decimal` | Float type | `real y = 3.14` |
| `str` | `string`, `text` | String type | `text name = "Alice"` |
| `bool` | `boolean`, `flag` | Boolean type | `flag active = yes` |
| `var` | `auto`, `any` | Universal type | `var data = "anything"` |

#### Boolean Values
| Keyword | Aliases | Description |
|---------|---------|-------------|
| `true` | `yes` | Boolean true |
| `false` | `no` | Boolean false |

### Operators

#### Arithmetic Operators
| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition | `a + b` |
| `-` | Subtraction | `a - b` |
| `*` | Multiplication | `a * b` |
| `/` | Division | `a / b` |
| `%` | Modulo | `a % b` |

#### Comparison Operators
| Operator | Description | Example |
|----------|-------------|---------|
| `==` | Equal | `a == b` |
| `!=` | Not equal | `a != b` |
| `<` | Less than | `a < b` |
| `<=` | Less than or equal | `a <= b` |
| `>` | Greater than | `a > b` |
| `>=` | Greater than or equal | `a >= b` |

#### Logical Operators
| Operator | Description | Example |
|----------|-------------|---------|
| `&&` | Logical AND | `a && b` |
| `\|\|` | Logical OR | `a \|\| b` |
| `!` | Logical NOT | `!a` |

#### Stream Operators
| Operator | Description | Example |
|----------|-------------|---------|
| `<<` | Stream output | `"Value: " << x` |
| `>>` | Format conversion | `x >> hex` |

## Input/Output

### Console Output

#### `print(expression)` / `say expression` / `show expression`
Print value to console with newline.

**Parameters:**
- `expression`: Any expression to print

**Examples:**
```dis
say "Hello World"
print(42)
show "Value: " << x << "\n"
```

#### Stream Output
Print multiple values in one statement using `<<` operator.

**Syntax:** `expression << value << value ...`

**Examples:**
```dis
show "Player: " << name << " Score: " << score
say "Position: (" << x << ", " << y << ")"
```

### Console Input

#### `input(prompt)` / `ask prompt -> variable`
Get user input with optional prompt.

**Parameters:**
- `prompt`: String to display to user
- `variable`: Variable to store result

**Examples:**
```dis
ask "Enter name:" -> name
get "Age?" -> age
read "Score?" -> score
```

#### Typed Input
Input with automatic type conversion.

**Examples:**
```dis
num age
ask "Age?" -> age        // Converts to integer

real height
get "Height?" -> height  // Converts to float

flag ready
read "Ready? (yes/no)" -> ready  // Converts to boolean
```

## Graphics System

### Window Management

#### `window(title, width, height)` / `win title, width, height`
Create a game window.

**Parameters:**
- `title`: Window title (string)
- `width`: Window width in pixels (integer)
- `height`: Window height in pixels (integer)

**Examples:**
```dis
win "My Game", 800, 600
window("3D Demo", 1024, 768)
```

#### `window_should_close()` / `should_close()` / `closed()`
Check if window should close (ESC pressed or X clicked).

**Returns:** Boolean indicating if window should close

**Example:**
```dis
repeat frame from 0 to 100000 {
    when (should_close()) break
    // Game logic
    flip()
}
```

### Graphics Initialization

#### `gl_init()` / `opengl()` / `init_gl()`
Initialize OpenGL graphics system.

**Example:**
```dis
win "Game", 800, 600
opengl()
```

#### `set_projection_2d()` / `2d()`
Set up 2D orthographic projection.

**Example:**
```dis
opengl()
2d()  // Now ready for 2D graphics
```

#### `set_projection_3d(fov, near, far)` / `3d(fov, near, far)`
Set up 3D perspective projection.

**Parameters:**
- `fov`: Field of view in degrees (float)
- `near`: Near clipping plane distance (float)
- `far`: Far clipping plane distance (float)

**Example:**
```dis
opengl()
3d(60.0, 0.1, 100.0)  // 60° FOV, near=0.1, far=100
```

### Rendering Control

#### `clear()` / `cls()`
Clear the screen/framebuffer.

**Example:**
```dis
cls()  // Clear screen to background color
```

#### `render()` / `flip()` / `swap()`
Display the rendered frame (swap buffers).

**Example:**
```dis
cls()
// Draw graphics here
flip()  // Display the frame
```

#### `set_clear_color(r, g, b, a)` / `bg_color(r, g, b, a)`
Set background clear color.

**Parameters:**
- `r`: Red component (0.0 to 1.0)
- `g`: Green component (0.0 to 1.0)
- `b`: Blue component (0.0 to 1.0)
- `a`: Alpha component (0.0 to 1.0, optional)

**Example:**
```dis
bg_color 0.2, 0.3, 0.5, 1.0  // Dark blue background
```

### Colors

#### `set_color(r, g, b)` / `color(r, g, b)`
Set current drawing color.

**Parameters:**
- `r`: Red component (0.0 to 1.0)
- `g`: Green component (0.0 to 1.0)
- `b`: Blue component (0.0 to 1.0)

**Examples:**
```dis
color 1.0, 0.0, 0.0  // Red
color 0.0, 1.0, 0.0  // Green
color 0.0, 0.0, 1.0  // Blue
color 1.0, 1.0, 1.0  // White
```

### 2D Drawing Functions

#### `draw_pixel(x, y)` / `pixel(x, y)`
Draw a single pixel.

**Parameters:**
- `x`: X coordinate (integer)
- `y`: Y coordinate (integer)

**Example:**
```dis
color 1.0, 0.0, 0.0
pixel 100, 100  // Red pixel at (100, 100)
```

#### `draw_line(x1, y1, x2, y2)` / `line(x1, y1, x2, y2)`
Draw a line between two points.

**Parameters:**
- `x1, y1`: Start point coordinates
- `x2, y2`: End point coordinates

**Example:**
```dis
line 0, 0, 100, 100  // Diagonal line
```

#### `draw_rect(x, y, width, height)` / `rect(x, y, width, height)`
Draw rectangle outline.

**Parameters:**
- `x, y`: Top-left corner position
- `width, height`: Rectangle dimensions

**Example:**
```dis
rect 50, 50, 100, 75  // Rectangle outline
```

#### `fill_rect(x, y, width, height)` / `box(x, y, width, height)`
Draw filled rectangle.

**Parameters:**
- `x, y`: Top-left corner position
- `width, height`: Rectangle dimensions

**Example:**
```dis
box 50, 50, 100, 75  // Filled rectangle
```

#### `draw_circle(cx, cy, radius)` / `circle(cx, cy, radius)`
Draw circle outline.

**Parameters:**
- `cx, cy`: Center coordinates
- `radius`: Circle radius

**Example:**
```dis
circle 200, 200, 50  // Circle outline
```

#### `fill_circle(cx, cy, radius)` / `ball(cx, cy, radius)`
Draw filled circle.

**Parameters:**
- `cx, cy`: Center coordinates
- `radius`: Circle radius

**Example:**
```dis
ball 200, 200, 50  // Filled circle
```

### 3D Drawing Functions

#### `draw_triangle(x1, y1, x2, y2, x3, y3)` / `triangle(x1, y1, x2, y2, x3, y3)`
Draw 3D triangle (in 2D mode) or 3D triangle (in 3D mode).

**Parameters:**
- `x1, y1, x2, y2, x3, y3`: Three vertex coordinates

**Example:**
```dis
triangle 0, 1, 0, -1, -1, 0, 1, -1, 0  // 3D triangle
```

#### `draw_quad(x, y, width, height)` / `quad(x, y, width, height)`
Draw 3D quad/rectangle.

**Parameters:**
- `x, y`: Position
- `width, height`: Dimensions

**Example:**
```dis
quad -1, -1, 2, 2  // 2x2 quad centered at origin
```

### 3D Transformations

#### `translate(x, y, z)` / `move(x, y, z)`
Translate (move) coordinate system.

**Parameters:**
- `x, y, z`: Translation amounts

**Example:**
```dis
move 2, 1, -5  // Move 2 right, 1 up, 5 back
```

#### `rotate(angle, x, y, z)` / `turn(angle, x, y, z)`
Rotate coordinate system around axis.

**Parameters:**
- `angle`: Rotation angle in degrees
- `x, y, z`: Rotation axis (normalized vector)

**Examples:**
```dis
turn 45, 0, 1, 0    // Rotate 45° around Y axis
turn 90, 1, 0, 0    // Rotate 90° around X axis
```

#### `scale(x, y, z)` / `size(x, y, z)`
Scale coordinate system.

**Parameters:**
- `x, y, z`: Scale factors

**Example:**
```dis
size 2, 2, 2  // Scale 2x in all directions
```

#### `push_matrix()` / `push()`
Save current transformation matrix.

**Example:**
```dis
push()
move 5, 0, 0
// Draw something at offset position
pop()  // Restore original position
```

#### `pop_matrix()` / `pop()`
Restore previously saved transformation matrix.

**Example:**
```dis
push()
// Apply transformations
// Draw objects
pop()  // Back to original transformation
```

## Math Functions

### Basic Math
Currently, basic math operations are handled by operators. Advanced math functions can be implemented in libraries.

**Built-in operators:**
- `+`, `-`, `*`, `/`, `%` (modulo)

**Example library functions:**
```dis
// These would be in a math library
def sqrt(real x) { /* implementation */ }
def sin(real x) { /* implementation */ }
def cos(real x) { /* implementation */ }
def abs(real x) { /* implementation */ }
```

## String Operations

### String Literals
```dis
text message = "Hello World"
text multiline = "Line 1\nLine 2\nLine 3"
text escaped = "Quote: \"Hello\" and backslash: \\"
```

### String Arrays
```dis
text chars[] = "Hello"  // ['H', 'e', 'l', 'l', 'o', '\0']
text first = chars[0]   // 'H'
chars[0] = 'B'         // Changes to "Bello"
```

### String Concatenation
```dis
text greeting = "Hello " << name << "!"
show "Score: " << score << " points"
```

## System Functions

### Input Handling

#### `keypress()` / `key()` / `get_keypress()`
Get keyboard input (blocking).

**Returns:** Integer key code or character

**Example:**
```dis
num key = keypress()
when (key == 'w') {
    // Move up
}
```

#### `hotkey(key, function)` / `register_hotkey(key, function)`
Register hotkey callback.

**Parameters:**
- `key`: Key character or code
- `function`: Function to call when key pressed

**Example:**
```dis
def quit_game() {
    say "Goodbye!"
}

hotkey('q', quit_game)
```

### Network Functions

#### `http(url)` / `web(url)`
Make HTTP GET request.

**Parameters:**
- `url`: URL to request (string)

**Returns:** Response body as string

**Example:**
```dis
text response = web("https://api.example.com/data")
show "Response: " << response
```

### AI Functions

#### `ai(prompt)` / `ai_query(prompt)`
Query AI system (placeholder).

**Parameters:**
- `prompt`: Question or prompt (string)

**Returns:** AI response as string

**Example:**
```dis
text answer = ai("What is 2 + 2?")
show "AI says: " << answer
```

## Compilation Directives

### File Inclusion

#### `include "filename"`
Include source file at compile time.

**Parameters:**
- `filename`: Path to .dis or .dish file

**Example:**
```dis
include "math_utils.dis"
include "graphics_helpers.dish"
```

#### Binary Library Inclusion
```dis
use "math.disll"  // Include binary library
```

### Compilation Targets

#### Command Line Options
```bash
# Native executable (default)
disp program.dis -o program.exe

# C source code
disp program.dis -emit-c -o program.c

# Assembly code
disp program.dis -emit-asm -o program.s

# WebAssembly
disp program.dis -emit-wasm -o program.wat

# Bare metal assembly
disp program.dis -bare-asm -o boot.s

# Binary library
disp library.dis -emit-lib -o library.disll

# Packaged executable
disp program.dis -emit-disp -o program.disp
```

### Format Specifiers

#### Output Formatting
```dis
num x = 255
x >> hex    // Output as hexadecimal
x >> bin    // Output as binary
x >> oct    // Output as octal
x >> dec    // Output as decimal (default)
```

#### Format Conversion
```dis
text hex_str = "FF"
num value = hex_str >> hex  // Convert hex string to number
```

## Error Handling

### Compilation Errors
The compiler provides detailed error messages with:
- Line and column numbers
- Source code context
- Suggested fixes

### Runtime Behavior
- Array bounds are not automatically checked
- Division by zero behavior is undefined
- Memory management is automatic for basic types

## Platform Differences

### Windows
- Full OpenGL support
- WinAPI for window management
- WinINet for HTTP requests

### Linux/macOS
- OpenGL support (requires X11/OpenGL libraries)
- POSIX APIs for system functions
- libcurl for HTTP requests

### WebAssembly
- Limited graphics (canvas-based)
- WASI for system calls
- No direct file system access

This API reference covers all currently implemented features. The language is actively developed, so new functions and capabilities are regularly added.