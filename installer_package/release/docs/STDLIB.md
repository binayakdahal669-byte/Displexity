# Displexity Standard Library

Documentation for built-in libraries and standard functions available in Displexity.

## Table of Contents

1. [Core Library](#core-library)
2. [Math Library](#math-library)
3. [Graphics Library](#graphics-library)
4. [String Library](#string-library)
5. [System Library](#system-library)
6. [Game Library](#game-library)
7. [Creating Custom Libraries](#creating-custom-libraries)

## Core Library

The core library is automatically available in all Displexity programs.

### Type System Functions

#### Type Checking
```dis
def is_number(var x) {
    // Check if variable contains a number
    give (x.ival != 0 || x.str == "0")
}

def is_string(var x) {
    // Check if variable contains text
    give (x.str != "")
}

def get_type(var x) {
    // Return type as string
    when (is_number(x)) give "number"
    when (is_string(x)) give "string"
    give "unknown"
}
```

#### Type Conversion
```dis
def to_int(var x) {
    give x.ival
}

def to_float(var x) {
    give x.fval
}

def to_string(var x) {
    give x.str
}
```

### Array Utilities
```dis
def array_length(num[] arr, num max_size) {
    // Find actual length of null-terminated array
    repeat i from 0 to max_size {
        when (arr[i] == 0) give i
    }
    give max_size
}

def array_sum(num[] arr, num size) {
    num total = 0
    repeat i from 0 to size {
        total = total + arr[i]
    }
    give total
}

def array_max(num[] arr, num size) {
    num max_val = arr[0]
    repeat i from 1 to size {
        when (arr[i] > max_val) {
            max_val = arr[i]
        }
    }
    give max_val
}

def array_min(num[] arr, num size) {
    num min_val = arr[0]
    repeat i from 1 to size {
        when (arr[i] < min_val) {
            min_val = arr[i]
        }
    }
    give min_val
}
```

## Math Library

Advanced mathematical functions. Include with: `include "math.dis"`

### Basic Math Functions
```dis
def abs(real x) {
    when (x < 0) give -x
    give x
}

def min(real a, real b) {
    when (a < b) give a
    give b
}

def max(real a, real b) {
    when (a > b) give a
    give b
}

def clamp(real value, real min_val, real max_val) {
    when (value < min_val) give min_val
    when (value > max_val) give max_val
    give value
}
```

### Advanced Math Functions
```dis
def sqrt(real x) {
    // Newton's method for square root
    when (x < 0) give 0
    when (x == 0) give 0
    
    real guess = x / 2
    repeat i from 0 to 20 {  // 20 iterations for precision
        real new_guess = (guess + x / guess) / 2
        when (abs(new_guess - guess) < 0.0001) {
            give new_guess
        }
        guess = new_guess
    }
    give guess
}

def power(real base, num exponent) {
    real result = 1
    repeat i from 0 to exponent {
        result = result * base
    }
    give result
}

def factorial(num n) {
    when (n <= 1) give 1
    give n * factorial(n - 1)
}
```

### Trigonometric Functions
```dis
// Constants
real PI = 3.14159265359
real PI_2 = 1.57079632679  // PI/2
real PI_4 = 0.78539816339  // PI/4

def degrees_to_radians(real degrees) {
    give degrees * PI / 180.0
}

def radians_to_degrees(real radians) {
    give radians * 180.0 / PI
}

def sin(real x) {
    // Taylor series approximation
    real result = 0
    real term = x
    real x_squared = x * x
    
    repeat i from 0 to 10 {
        result = result + term
        term = term * (-x_squared) / ((2 * i + 2) * (2 * i + 3))
    }
    give result
}

def cos(real x) {
    give sin(PI_2 - x)
}

def tan(real x) {
    real cos_x = cos(x)
    when (abs(cos_x) < 0.0001) give 1000000  // Large number for infinity
    give sin(x) / cos_x
}
```

### Random Number Generation
```dis
num random_seed = 1

def set_random_seed(num seed) {
    random_seed = seed
}

def random() {
    // Linear congruential generator
    random_seed = (random_seed * 1103515245 + 12345) % 2147483648
    give random_seed
}

def random_range(num min_val, num max_val) {
    num range = max_val - min_val + 1
    give min_val + (random() % range)
}

def random_float() {
    give random() / 2147483648.0
}

def random_float_range(real min_val, real max_val) {
    give min_val + random_float() * (max_val - min_val)
}
```

## Graphics Library

Extended graphics functions. Include with: `include "graphics.dis"`

### Color Utilities
```dis
def rgb_to_hsv(real r, real g, real b) {
    // Convert RGB to HSV color space
    real max_val = max(max(r, g), b)
    real min_val = min(min(r, g), b)
    real delta = max_val - min_val
    
    // Value
    real v = max_val
    
    // Saturation
    real s = 0
    when (max_val > 0) {
        s = delta / max_val
    }
    
    // Hue
    real h = 0
    when (delta > 0) {
        when (max_val == r) {
            h = 60 * ((g - b) / delta)
        } otherwise when (max_val == g) {
            h = 60 * (2 + (b - r) / delta)
        } otherwise {
            h = 60 * (4 + (r - g) / delta)
        }
    }
    
    when (h < 0) h = h + 360
    
    // Return as array (conceptual)
    show "HSV: " << h << ", " << s << ", " << v
}

def lerp_color(real r1, real g1, real b1, real r2, real g2, real b2, real t) {
    // Linear interpolation between two colors
    real r = r1 + (r2 - r1) * t
    real g = g1 + (g2 - g1) * t
    real b = b1 + (b2 - b1) * t
    color r, g, b
}
```

### Drawing Utilities
```dis
def draw_gradient_rect(num x, num y, num width, num height,
                       real r1, real g1, real b1, real r2, real g2, real b2) {
    // Draw rectangle with vertical gradient
    repeat i from 0 to height {
        real t = i / height
        lerp_color r1, g1, b1, r2, g2, b2, t
        line x, y + i, x + width, y + i
    }
}

def draw_circle_outline(num cx, num cy, num radius, num thickness) {
    // Draw thick circle outline
    repeat i from 0 to thickness {
        circle cx, cy, radius + i
    }
}

def draw_rounded_rect(num x, num y, num width, num height, num corner_radius) {
    // Draw rectangle with rounded corners
    // Main rectangle
    box x + corner_radius, y, width - 2 * corner_radius, height
    box x, y + corner_radius, width, height - 2 * corner_radius
    
    // Corner circles
    ball x + corner_radius, y + corner_radius, corner_radius
    ball x + width - corner_radius, y + corner_radius, corner_radius
    ball x + corner_radius, y + height - corner_radius, corner_radius
    ball x + width - corner_radius, y + height - corner_radius, corner_radius
}

def draw_star(num cx, num cy, num outer_radius, num inner_radius, num points) {
    // Draw star shape
    real angle_step = 360.0 / (points * 2)
    
    repeat i from 0 to points * 2 {
        real angle = i * angle_step * PI / 180.0
        real radius = outer_radius
        when (i % 2 == 1) radius = inner_radius
        
        real x = cx + cos(angle) * radius
        real y = cy + sin(angle) * radius
        
        when (i == 0) {
            // Move to first point (conceptual)
        } otherwise {
            // Draw line to point (would need line drawing)
        }
    }
}
```

### Animation Helpers
```dis
def ease_in_out(real t) {
    // Smooth easing function
    give t * t * (3.0 - 2.0 * t)
}

def bounce(real t) {
    // Bounce easing function
    when (t < 0.5) {
        give 2 * t * t
    } otherwise {
        give 1 - 2 * (1 - t) * (1 - t)
    }
}

def animate_value(real start, real end, real progress) {
    // Linear interpolation
    give start + (end - start) * progress
}

def animate_position(real start_x, real start_y, real end_x, real end_y, real progress) {
    real x = animate_value(start_x, end_x, progress)
    real y = animate_value(start_y, end_y, progress)
    // Return position (conceptual)
    show "Position: " << x << ", " << y
}
```

## String Library

String manipulation functions. Include with: `include "strings.dis"`

### String Operations
```dis
def string_length(text str) {
    num length = 0
    repeat i from 0 to 1000 {  // Max string length
        when (str[i] == 0) {  // Null terminator
            give length
        }
        length = length + 1
    }
    give length
}

def string_copy(text dest, text src, num max_length) {
    repeat i from 0 to max_length {
        dest[i] = src[i]
        when (src[i] == 0) break
    }
}

def string_compare(text str1, text str2) {
    repeat i from 0 to 1000 {
        when (str1[i] != str2[i]) {
            give str1[i] - str2[i]
        }
        when (str1[i] == 0) give 0  // Equal strings
    }
    give 0
}

def string_find(text haystack, text needle) {
    num haystack_len = string_length(haystack)
    num needle_len = string_length(needle)
    
    repeat i from 0 to haystack_len - needle_len + 1 {
        flag found = yes
        repeat j from 0 to needle_len {
            when (haystack[i + j] != needle[j]) {
                found = no
                break
            }
        }
        when (found) give i
    }
    give -1  // Not found
}
```

### String Conversion
```dis
def string_to_upper(text str) {
    num len = string_length(str)
    repeat i from 0 to len {
        when (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32  // Convert to uppercase
        }
    }
}

def string_to_lower(text str) {
    num len = string_length(str)
    repeat i from 0 to len {
        when (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32  // Convert to lowercase
        }
    }
}

def char_is_digit(text c) {
    give (c >= '0' && c <= '9')
}

def char_is_alpha(text c) {
    give ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
}

def string_to_int(text str) {
    num result = 0
    num sign = 1
    num i = 0
    
    // Handle negative sign
    when (str[0] == '-') {
        sign = -1
        i = 1
    }
    
    repeat j from i to string_length(str) {
        when (char_is_digit(str[j])) {
            result = result * 10 + (str[j] - '0')
        } otherwise {
            break
        }
    }
    
    give result * sign
}
```

## System Library

System interaction functions. Include with: `include "system.dis"`

### File Operations (Conceptual)
```dis
def file_exists(text filename) {
    // Check if file exists (would need system call)
    // Placeholder implementation
    give yes
}

def read_file(text filename) {
    // Read entire file into string (would need system call)
    // Placeholder implementation
    give "file contents"
}

def write_file(text filename, text content) {
    // Write string to file (would need system call)
    // Placeholder implementation
    say "Writing to " << filename
}
```

### Time Functions
```dis
num start_time = 0

def get_time() {
    // Get current time in milliseconds (would need system call)
    // Placeholder: use frame counter
    give start_time
}

def sleep(num milliseconds) {
    // Sleep for specified time (would need system call)
    // Placeholder implementation
    say "Sleeping for " << milliseconds << "ms"
}

def start_timer() {
    start_time = get_time()
}

def get_elapsed_time() {
    give get_time() - start_time
}
```

## Game Library

Game development utilities. Include with: `include "game.dis"`

### Collision Detection
```dis
def point_in_rect(num px, num py, num rx, num ry, num rw, num rh) {
    give (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh)
}

def rect_collision(num x1, num y1, num w1, num h1,
                   num x2, num y2, num w2, num h2) {
    give (x1 < x2 + w2 && x1 + w1 > x2 && 
          y1 < y2 + h2 && y1 + h1 > y2)
}

def circle_collision(num x1, num y1, num r1,
                     num x2, num y2, num r2) {
    num dx = x1 - x2
    num dy = y1 - y2
    num distance_squared = dx * dx + dy * dy
    num radius_sum = r1 + r2
    give (distance_squared <= radius_sum * radius_sum)
}

def point_in_circle(num px, num py, num cx, num cy, num radius) {
    num dx = px - cx
    num dy = py - cy
    give (dx * dx + dy * dy <= radius * radius)
}
```

### Vector Math
```dis
def vector_length(real x, real y) {
    give sqrt(x * x + y * y)
}

def vector_normalize(real x, real y) {
    real length = vector_length(x, y)
    when (length > 0) {
        show "Normalized: " << (x / length) << ", " << (y / length)
    } otherwise {
        show "Normalized: 0, 0"
    }
}

def vector_dot(real x1, real y1, real x2, real y2) {
    give x1 * x2 + y1 * y2
}

def vector_distance(real x1, real y1, real x2, real y2) {
    real dx = x2 - x1
    real dy = y2 - y1
    give sqrt(dx * dx + dy * dy)
}
```

### Game State Management
```dis
num game_state = 0
num previous_state = 0

def set_game_state(num new_state) {
    previous_state = game_state
    game_state = new_state
}

def get_game_state() {
    give game_state
}

def restore_previous_state() {
    num temp = game_state
    game_state = previous_state
    previous_state = temp
}
```

### Entity System
```dis
// Simple entity management
num entity_x[100], entity_y[100]
num entity_type[100]
flag entity_active[100]
num entity_count = 0

def create_entity(num x, num y, num type) {
    when (entity_count < 100) {
        entity_x[entity_count] = x
        entity_y[entity_count] = y
        entity_type[entity_count] = type
        entity_active[entity_count] = yes
        entity_count = entity_count + 1
        give entity_count - 1  // Return entity ID
    }
    give -1  // Failed to create
}

def destroy_entity(num id) {
    when (id >= 0 && id < entity_count) {
        entity_active[id] = no
    }
}

def update_entities() {
    repeat i from 0 to entity_count {
        when (entity_active[i]) {
            // Update entity based on type
            when (entity_type[i] == 1) {
                // Update player
            } otherwise when (entity_type[i] == 2) {
                // Update enemy
            }
        }
    }
}
```

## Creating Custom Libraries

### Library Structure
```dis
// math_extended.dis - Custom math library

// Library initialization
def init_math_extended() {
    say "Math Extended Library v1.0 loaded"
}

// Public functions
def advanced_sqrt(real x, num precision) {
    // More precise square root
    real guess = x / 2
    repeat i from 0 to precision {
        guess = (guess + x / guess) / 2
    }
    give guess
}

def fibonacci(num n) {
    when (n <= 1) give n
    give fibonacci(n - 1) + fibonacci(n - 2)
}

// Private helper functions
def _internal_helper(num x) {
    // Internal function (prefix with _)
    give x * 2
}
```

### Using Custom Libraries
```dis
// main.dis
include "math_extended.dis"

def main() {
    init_math_extended()
    
    real result = advanced_sqrt(16.0, 10)
    show "Precise sqrt(16) = " << result
    
    num fib_10 = fibonacci(10)
    show "Fibonacci(10) = " << fib_10
}
```

### Binary Library Creation
```bash
# Compile library to binary format
disp math_extended.dis -emit-lib -o math_extended.disll

# Use binary library
# In your main program:
# include "math_extended.disll"
```

### Library Best Practices

1. **Naming Conventions**
   - Use descriptive function names
   - Prefix private functions with `_`
   - Use consistent parameter naming

2. **Documentation**
   - Comment all public functions
   - Provide usage examples
   - Document parameter types and return values

3. **Error Handling**
   - Check parameter validity
   - Provide meaningful error messages
   - Use return codes for error indication

4. **Performance**
   - Avoid recursive functions for large inputs
   - Use efficient algorithms
   - Consider memory usage

This standard library provides a foundation for common programming tasks in Displexity. Libraries can be extended and customized for specific project needs.