# Displexity Programming Language

**A modern, intuitive programming language for game development, system programming, and creative coding.**

## 🚀 Quick Start

```bash
# Compile and run a program
disp hello.dis

# Compile to executable
disp game.dis -o game.exe

# Multi-file project
disp main.dis utils.dis graphics.dis -o project.exe
```

## ✨ Features

- **🎮 Game Engine Built-in** - OpenGL graphics, physics, input handling
- **🔧 Multiple Targets** - Native executables, WebAssembly, bare metal
- **📦 Binary Libraries** - Compile to `.disll` libraries for code reuse
- **🌐 Multi-file Projects** - Organize large codebases easily
- **🎯 Simple Syntax** - Python-like readability with C-like performance
- **⚡ Turing Complete** - Full computational power for any application

## 📖 Documentation

- [Language Guide](LANGUAGE_GUIDE.md) - Complete syntax reference
- [Game Development](GAME_DEV.md) - Graphics, physics, and game programming
- [Standard Library](STDLIB.md) - Built-in functions and modules
- [Examples](../examples/) - Sample programs and tutorials
- [API Reference](API_REFERENCE.md) - Complete function documentation

## 🎯 Hello World

```dis
print("Hello, World!")
```

## 🎮 Simple Game

```dis
window("My Game", 800, 600)
opengl()
projection_2d()

for frame in range(1000):
    clear()
    color(1.0, 0.0, 0.0)
    rect(100 + frame, 200, 50, 50)
    render()
```

## 🎲 Advanced Example - 3D Game

```dis
# 3D spinning cube with lighting
window("3D Demo", 1024, 768)
opengl()
projection_3d(60.0, 0.1, 100.0)

for frame in range(2000):
    clear()
    # Set background color
    
    push_matrix()
    translate(0, 0, -5)
    rotate(frame * 2, 1, 1, 0)
    
    color(1.0, 0.5, 0.2)
    # Draw cube faces
    rect(-1, -1, 2, 2)  # Simple cube representation
    
    pop_matrix()
    render()
```

## 📊 Language Comparison

| Feature | Displexity | Python | C++ | JavaScript |
|---------|------------|--------|-----|------------|
| Game Graphics | ✅ Built-in | ❌ External | ❌ External | ❌ External |
| Simple Syntax | ✅ | ✅ | ❌ | ✅ |
| Performance | ✅ Native | ❌ Interpreted | ✅ Native | ❌ JIT |
| Multi-target | ✅ | ❌ | ❌ | ❌ |
| Learning Curve | 🟢 Easy | 🟢 Easy | 🔴 Hard | 🟡 Medium |

## 🛠 Installation

### Windows Installer
1. Download `displexity-installer.exe`
2. Run installer as administrator
3. Use `disp` command from any directory

### Manual Installation
1. Extract release package to `C:\Displexity`
2. Add `C:\Displexity\bin` to system PATH
3. Run `disp --help` to verify installation

### Build from Source
```bash
git clone https://github.com/displexity/displexity
cd displexity
./build.bat    # Windows
./build.sh     # Linux/macOS
```

## 🎓 Learning Path

### 1. Basics (30 minutes)
- Variables: `num x = 42`, `text name = "Alice"`
- Output: `say "Hello"`, `show x`
- Input: `ask "Name?" -> name`

### 2. Control Flow (45 minutes)
- Conditions: `when (x > 10) { ... } otherwise { ... }`
- Loops: `repeat i from 0 to 10 { ... }`
- Functions: `def greet(text name) { say "Hi " << name }`

### 3. Graphics (1 hour)
- Windows: `win "Game", 800, 600`
- 2D Graphics: `2d()`, `box()`, `circle()`, `color()`
- 3D Graphics: `3d()`, `move()`, `turn()`, `scale()`

### 4. Game Development (2 hours)
- Game loops with `repeat` and `flip()`
- Input handling and collision detection
- Physics simulation and animations

### 5. Advanced Features (1 hour)
- Multi-file projects with `include`
- Binary libraries (`.disll` files)
- Multiple compilation targets

## 🎮 Example Projects

### Pong Game
```dis
win "Pong", 800, 600
opengl()
2d()

num paddle1_y = 250, paddle2_y = 250
num ball_x = 400, ball_y = 300
num ball_dx = 5, ball_dy = 3

repeat frame from 0 to 10000 {
    cls()
    
    // Update ball
    ball_x = ball_x + ball_dx
    ball_y = ball_y + ball_dy
    
    // Bounce off walls
    when (ball_y < 0 || ball_y > 600) ball_dy = -ball_dy
    
    // Draw game
    color 1, 1, 1
    box 50, paddle1_y, 20, 100      // Left paddle
    box 730, paddle2_y, 20, 100     // Right paddle
    box ball_x, ball_y, 15, 15      // Ball
    
    flip()
}
```

### Fractal Generator
```dis
win "Mandelbrot", 800, 600
opengl()
2d()

repeat y from 0 to 600 {
    repeat x from 0 to 800 {
        real cx = (x - 400) / 200.0
        real cy = (y - 300) / 200.0
        
        real zx = 0, zy = 0
        num iter = 0
        
        repeat i from 0 to 100 {
            real zx2 = zx * zx - zy * zy + cx
            real zy2 = 2 * zx * zy + cy
            zx = zx2
            zy = zy2
            
            when (zx*zx + zy*zy > 4) {
                iter = i
                break
            }
        }
        
        real c = iter / 100.0
        color c, c * 0.5, c * 0.8
        pixel x, y
    }
    
    when (y % 50 == 0) flip()  // Update display periodically
}
```

## 🤝 Community & Support

- **GitHub**: [github.com/displexity/displexity](https://github.com/displexity/displexity)
- **Discord**: Join our community server for help and discussions
- **Examples**: Browse 50+ examples in the `examples/` folder
- **Tutorials**: Step-by-step guides in `docs/tutorials/`
- **Wiki**: Community-maintained documentation and tips

## 🏆 Achievements

- ✅ **Turing Complete** - Can simulate any computer program
- ✅ **Game Engine** - Built-in OpenGL graphics and physics
- ✅ **Multi-Platform** - Windows, Linux, macOS, WebAssembly
- ✅ **Performance** - Compiles to native machine code
- ✅ **Simplicity** - Learn in under 2 hours

## 📄 License

MIT License - See [LICENSE](../LICENSE.txt) for details.

---

*"Making programming fun and accessible for everyone"* 🎯

**Ready to build the next GTA? Start with Displexity!** 🚀