# Game Development with Displexity

Complete guide to creating games using Displexity's built-in game engine.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Graphics System](#graphics-system)
3. [Input Handling](#input-handling)
4. [Game Loops](#game-loops)
5. [Physics and Animation](#physics-and-animation)
6. [Audio System](#audio-system)
7. [Game Examples](#game-examples)
8. [Performance Optimization](#performance-optimization)

## Getting Started

### Your First Game Window

```dis
// Create a game window
win "My First Game", 800, 600
opengl()  // Initialize OpenGL
2d()      // Set up 2D rendering

// Simple game loop
repeat frame from 0 to 1000 {
    cls()                    // Clear screen
    color 1.0, 0.0, 0.0     // Red color
    box 100, 100, 50, 50    // Draw red square
    flip()                  // Display frame
}
```

### Basic Game Structure

```dis
// Game initialization
def init_game() {
    win "Game Title", 1024, 768
    opengl()
    2d()
    // Initialize game variables
}

// Game loop
def game_loop() {
    repeat frame from 0 to 60000 {  // 1000 seconds at 60 FPS
        handle_input()
        update_game()
        render_game()
        flip()
    }
}

// Main function
def main() {
    init_game()
    game_loop()
}
```

## Graphics System

### 2D Graphics

#### Basic Shapes
```dis
2d()  // Set 2D projection

// Colors (RGB values 0.0 to 1.0)
color 1.0, 0.0, 0.0     // Red
color 0.0, 1.0, 0.0     // Green
color 0.0, 0.0, 1.0     // Blue
color 1.0, 1.0, 1.0     // White

// Shapes
pixel 100, 100                    // Single pixel
line 0, 0, 100, 100              // Line from (0,0) to (100,100)
box 50, 50, 100, 75              // Rectangle at (50,50), size 100x75
rect 200, 200, 80, 60            // Rectangle outline
circle 300, 300, 50              // Circle at (300,300), radius 50
ball 400, 400, 30                // Filled circle
```

#### Advanced 2D Graphics
```dis
// Gradient effects (simulate with multiple rectangles)
repeat i from 0 to 100 {
    real intensity = i / 100.0
    color intensity, 0.0, 1.0 - intensity
    box i * 8, 100, 8, 200
}

// Sprite-like rendering with colored rectangles
def draw_player(num x, num y) {
    // Head
    color 1.0, 0.8, 0.6  // Skin color
    circle x, y - 20, 10
    
    // Body
    color 0.0, 0.0, 1.0  // Blue shirt
    box x - 8, y - 10, 16, 30
    
    // Arms
    color 1.0, 0.8, 0.6  // Skin color
    box x - 15, y - 5, 7, 20
    box x + 8, y - 5, 7, 20
    
    // Legs
    color 0.2, 0.2, 0.8  // Dark blue pants
    box x - 8, y + 20, 7, 25
    box x + 1, y + 20, 7, 25
}
```

### 3D Graphics

#### 3D Setup
```dis
3d(60.0, 0.1, 100.0)  // FOV=60°, near=0.1, far=100.0

// Camera positioning
push()
move 0, 0, -10        // Move camera back
turn 30, 1, 0, 0      // Rotate camera down 30°
```

#### 3D Transformations
```dis
// Matrix operations
push()                // Save current transformation
move 2, 1, -5        // Translate
turn 45, 0, 1, 0     // Rotate 45° around Y axis
size 2, 2, 2         // Scale 2x in all directions

// Draw object with transformations applied
box -1, -1, -1, 2, 2, 2  // 2x2x2 cube

pop()                 // Restore transformation
```

#### 3D Shapes
```dis
// 3D primitives
box -1, -1, -1, 2, 2, 2              // Cube
triangle 0, 1, 0, -1, -1, 0, 1, -1, 0  // 3D triangle

// Complex 3D objects (built from primitives)
def draw_pyramid(num x, num y, num z, num size) {
    push()
    move x, y, z
    
    // Base
    color 0.8, 0.6, 0.4
    box -size, -size, -size, size*2, 0.1, size*2
    
    // Sides (4 triangles)
    color 0.9, 0.7, 0.5
    triangle 0, size, 0, -size, 0, -size, size, 0, -size
    triangle 0, size, 0, size, 0, -size, size, 0, size
    triangle 0, size, 0, size, 0, size, -size, 0, size
    triangle 0, size, 0, -size, 0, size, -size, 0, -size
    
    pop()
}
```

### Lighting and Materials

```dis
// Basic lighting simulation with color variation
def set_lighting(real ambient, real diffuse) {
    // Simulate lighting by adjusting color intensity
    color ambient + diffuse, ambient + diffuse * 0.8, ambient + diffuse * 0.6
}

// Material properties (simulated)
def set_material_metal() {
    color 0.7, 0.7, 0.8  // Metallic gray
}

def set_material_wood() {
    color 0.6, 0.4, 0.2  // Brown wood
}
```

## Input Handling

### Keyboard Input
```dis
// Basic key detection (polling)
num key = keypress()
when (key == 'w') {
    player_y = player_y - 5  // Move up
}
when (key == 's') {
    player_y = player_y + 5  // Move down
}
when (key == 'a') {
    player_x = player_x - 5  // Move left
}
when (key == 'd') {
    player_x = player_x + 5  // Move right
}
```

### Advanced Input System
```dis
// Input state tracking
flag keys_pressed[256]  // Track all keys
num mouse_x = 0, mouse_y = 0

def update_input() {
    num key = keypress()
    when (key > 0 && key < 256) {
        keys_pressed[key] = yes
    }
    // Mouse input would be handled similarly
}

def is_key_down(num key_code) {
    give keys_pressed[key_code]
}

// Usage
when (is_key_down('w')) {
    move_player_up()
}
```

### Hotkey System
```dis
// Register hotkeys for specific actions
def setup_hotkeys() {
    hotkey('q', quit_game)
    hotkey('p', pause_game)
    hotkey('r', restart_game)
}

def quit_game() {
    say "Thanks for playing!"
    // Exit game
}
```

## Game Loops

### Basic Game Loop
```dis
def game_loop() {
    num running = yes
    
    repeat frame from 0 to 100000 {
        when (!running) break
        
        // Input phase
        handle_input()
        
        // Update phase
        update_entities()
        check_collisions()
        update_physics()
        
        // Render phase
        cls()
        draw_background()
        draw_entities()
        draw_ui()
        flip()
        
        // Frame rate control (built into flip())
    }
}
```

### State-Based Game Loop
```dis
num game_state = 0  // 0=menu, 1=playing, 2=paused, 3=game_over

def game_loop() {
    repeat frame from 0 to 100000 {
        when (game_state == 0) {
            update_menu()
            draw_menu()
        } otherwise when (game_state == 1) {
            update_gameplay()
            draw_gameplay()
        } otherwise when (game_state == 2) {
            draw_pause_screen()
        } otherwise when (game_state == 3) {
            update_game_over()
            draw_game_over()
        }
        flip()
    }
}
```

### Delta Time Game Loop
```dis
real last_time = 0
real delta_time = 0.016  // Assume 60 FPS initially

def game_loop() {
    repeat frame from 0 to 100000 {
        real current_time = frame * 0.016  // Approximate time
        delta_time = current_time - last_time
        last_time = current_time
        
        update_with_delta(delta_time)
        render()
        flip()
    }
}

def update_with_delta(real dt) {
    // Update positions based on time
    player_x = player_x + player_velocity_x * dt
    player_y = player_y + player_velocity_y * dt
}
```

## Physics and Animation

### Basic Physics
```dis
// Physics variables
real gravity = -500.0      // Pixels per second squared
real ground_y = 500.0      // Ground level

// Player physics
real player_x = 100, player_y = 100
real velocity_x = 0, velocity_y = 0
flag on_ground = no

def update_physics(real dt) {
    // Apply gravity
    when (!on_ground) {
        velocity_y = velocity_y + gravity * dt
    }
    
    // Update position
    player_x = player_x + velocity_x * dt
    player_y = player_y + velocity_y * dt
    
    // Ground collision
    when (player_y >= ground_y) {
        player_y = ground_y
        velocity_y = 0
        on_ground = yes
    } otherwise {
        on_ground = no
    }
}
```

### Collision Detection
```dis
// Rectangle collision
def rect_collision(num x1, num y1, num w1, num h1,
                   num x2, num y2, num w2, num h2) {
    give (x1 < x2 + w2 && x1 + w1 > x2 && 
          y1 < y2 + h2 && y1 + h1 > y2)
}

// Circle collision
def circle_collision(num x1, num y1, num r1,
                     num x2, num y2, num r2) {
    num dx = x1 - x2
    num dy = y1 - y2
    num distance_squared = dx * dx + dy * dy
    num radius_sum = r1 + r2
    give (distance_squared < radius_sum * radius_sum)
}

// Usage
when (rect_collision(player_x, player_y, 32, 32,
                     enemy_x, enemy_y, 32, 32)) {
    handle_player_enemy_collision()
}
```

### Animation System
```dis
// Sprite animation
num animation_frame = 0
num animation_speed = 10  // Frames per animation frame

def update_animation() {
    animation_frame = (animation_frame + 1) % (animation_speed * 4)
}

def draw_animated_sprite(num x, num y) {
    num sprite_frame = animation_frame / animation_speed
    
    // Different colors for different animation frames
    when (sprite_frame == 0) color 1.0, 0.0, 0.0  // Red
    when (sprite_frame == 1) color 0.0, 1.0, 0.0  // Green
    when (sprite_frame == 2) color 0.0, 0.0, 1.0  // Blue
    when (sprite_frame == 3) color 1.0, 1.0, 0.0  // Yellow
    
    box x, y, 32, 32
}
```

### Particle System
```dis
// Particle structure (simulated with arrays)
num particle_x[100], particle_y[100]
real particle_vx[100], particle_vy[100]
num particle_life[100]
num particle_count = 0

def create_particle(num x, num y, real vx, real vy) {
    when (particle_count < 100) {
        particle_x[particle_count] = x
        particle_y[particle_count] = y
        particle_vx[particle_count] = vx
        particle_vy[particle_count] = vy
        particle_life[particle_count] = 60  // 1 second at 60 FPS
        particle_count = particle_count + 1
    }
}

def update_particles(real dt) {
    repeat i from 0 to particle_count {
        when (particle_life[i] > 0) {
            // Update position
            particle_x[i] = particle_x[i] + particle_vx[i] * dt
            particle_y[i] = particle_y[i] + particle_vy[i] * dt
            
            // Apply gravity
            particle_vy[i] = particle_vy[i] + 200 * dt
            
            // Decrease life
            particle_life[i] = particle_life[i] - 1
        }
    }
}

def draw_particles() {
    repeat i from 0 to particle_count {
        when (particle_life[i] > 0) {
            real alpha = particle_life[i] / 60.0
            color alpha, alpha * 0.5, 0.0  // Fading orange
            pixel particle_x[i], particle_y[i]
        }
    }
}
```

## Audio System

Currently, Displexity focuses on graphics. Audio can be added through system calls or future extensions:

```dis
// Placeholder for future audio system
def play_sound(text filename) {
    // Future implementation
    say "Playing sound: " << filename
}

def play_music(text filename) {
    // Future implementation
    say "Playing music: " << filename
}
```

## Game Examples

### Pong Game
```dis
win "Pong", 800, 600
opengl()
2d()

// Game variables
num paddle1_y = 250, paddle2_y = 250
num ball_x = 400, ball_y = 300
real ball_vx = 200, ball_vy = 150  // Velocity in pixels/second
num score1 = 0, score2 = 0

def update_pong(real dt) {
    // Ball movement
    ball_x = ball_x + ball_vx * dt
    ball_y = ball_y + ball_vy * dt
    
    // Ball collision with top/bottom
    when (ball_y <= 0 || ball_y >= 600) {
        ball_vy = -ball_vy
    }
    
    // Ball collision with paddles
    when (ball_x <= 70 && ball_y >= paddle1_y && ball_y <= paddle1_y + 100) {
        ball_vx = -ball_vx
        ball_x = 70
    }
    when (ball_x >= 730 && ball_y >= paddle2_y && ball_y <= paddle2_y + 100) {
        ball_vx = -ball_vx
        ball_x = 730
    }
    
    // Scoring
    when (ball_x < 0) {
        score2 = score2 + 1
        reset_ball()
    }
    when (ball_x > 800) {
        score1 = score1 + 1
        reset_ball()
    }
    
    // Paddle AI (simple)
    when (ball_y > paddle2_y + 50) paddle2_y = paddle2_y + 100 * dt
    when (ball_y < paddle2_y + 50) paddle2_y = paddle2_y - 100 * dt
}

def draw_pong() {
    cls()
    color 1, 1, 1
    
    // Paddles
    box 50, paddle1_y, 20, 100
    box 730, paddle2_y, 20, 100
    
    // Ball
    circle ball_x, ball_y, 10
    
    // Center line
    repeat i from 0 to 30 {
        box 395, i * 20, 10, 10
    }
}

def reset_ball() {
    ball_x = 400
    ball_y = 300
    ball_vx = -ball_vx  // Reverse direction
}
```

### Platformer Game
```dis
win "Platformer", 1024, 768
opengl()
2d()

// Player variables
real player_x = 100, player_y = 600
real player_vx = 0, player_vy = 0
flag on_ground = no
num player_width = 32, player_height = 32

// Platform data
num platforms_x[] = [0, 200, 400, 600, 800]
num platforms_y[] = [700, 650, 600, 550, 500]
num platforms_w[] = [200, 150, 150, 150, 200]
num platforms_h[] = [50, 20, 20, 20, 50]

def update_platformer(real dt) {
    // Input
    num key = keypress()
    when (key == 'a') player_vx = -200  // Move left
    when (key == 'd') player_vx = 200   // Move right
    when (key == 'w' && on_ground) {    // Jump
        player_vy = -400
        on_ground = no
    }
    
    // Apply friction
    player_vx = player_vx * 0.8
    
    // Apply gravity
    when (!on_ground) {
        player_vy = player_vy + 800 * dt  // Gravity
    }
    
    // Update position
    player_x = player_x + player_vx * dt
    player_y = player_y + player_vy * dt
    
    // Platform collision
    on_ground = no
    repeat i from 0 to 5 {
        when (rect_collision(player_x, player_y, player_width, player_height,
                             platforms_x[i], platforms_y[i], 
                             platforms_w[i], platforms_h[i])) {
            when (player_vy > 0) {  // Falling
                player_y = platforms_y[i] - player_height
                player_vy = 0
                on_ground = yes
            }
        }
    }
    
    // Screen boundaries
    when (player_x < 0) player_x = 0
    when (player_x > 1024 - player_width) player_x = 1024 - player_width
}

def draw_platformer() {
    cls()
    bg_color 0.5, 0.8, 1.0, 1.0  // Sky blue
    
    // Draw platforms
    color 0.4, 0.2, 0.1  // Brown
    repeat i from 0 to 5 {
        box platforms_x[i], platforms_y[i], platforms_w[i], platforms_h[i]
    }
    
    // Draw player
    color 1.0, 0.0, 0.0  // Red
    box player_x, player_y, player_width, player_height
}
```

## Performance Optimization

### Efficient Rendering
```dis
// Batch similar operations
def draw_many_objects() {
    color 1.0, 0.0, 0.0  // Set color once
    repeat i from 0 to 100 {
        box i * 10, 100, 8, 8  // Draw many similar objects
    }
}

// Avoid unnecessary state changes
def optimized_draw() {
    // Group by color
    color 1.0, 0.0, 0.0
    draw_red_objects()
    
    color 0.0, 1.0, 0.0
    draw_green_objects()
    
    color 0.0, 0.0, 1.0
    draw_blue_objects()
}
```

### Memory Management
```dis
// Reuse objects instead of creating new ones
num bullet_pool_x[100], bullet_pool_y[100]
flag bullet_active[100]
num bullet_pool_size = 100

def fire_bullet(num x, num y) {
    repeat i from 0 to bullet_pool_size {
        when (!bullet_active[i]) {
            bullet_pool_x[i] = x
            bullet_pool_y[i] = y
            bullet_active[i] = yes
            break
        }
    }
}
```

### Frame Rate Management
```dis
// Skip expensive operations on some frames
num frame_counter = 0

def update_game() {
    frame_counter = frame_counter + 1
    
    // Update physics every frame
    update_physics()
    
    // Update AI every 3rd frame
    when (frame_counter % 3 == 0) {
        update_ai()
    }
    
    // Update particles every 2nd frame
    when (frame_counter % 2 == 0) {
        update_particles()
    }
}
```

This guide provides a comprehensive foundation for game development in Displexity. The built-in graphics system makes it easy to create both 2D and 3D games without external dependencies.