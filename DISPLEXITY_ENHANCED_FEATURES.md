# Displexity Language - Enhanced Features

## New Capabilities Added

### 1. FFI (Foreign Function Interface)

Call external C/C++ functions and libraries directly from Displexity:

```displexity
extern "C" {
    library("libm")
    
    func sin(float x) -> float
    func cos(float x) -> float
    func sqrt(float x) -> float
}

// Use external functions
float result = sin(3.14159)
print("sin(π) = " + result)
```

### 2. Module System

Create and use modular code:

```displexity
// mymodule.dis
module("mymodule")

export func add(int a, int b) {
    return a + b
}

export func multiply(int a, int b) {
    return a * b
}

// main.dis
import("mymodule")

int result = mymodule.add(5, 3)
print("5 + 3 = " + result)
```

### 3. REST API Client

Easy HTTP requests:

```displexity
// GET request
rest("GET", "/api/users") {
    headers {
        "Authorization": "Bearer token123"
        "Accept": "application/json"
    }
}

// POST request with JSON
rest("POST", "/api/users") {
    body {
        "name": "John",
        "email": "john@example.com"
    }
    headers {
        "Content-Type": "application/json"
    }
}
```

### 4. GraphQL Support

Query GraphQL APIs:

```displexity
graphql("https://api.example.com/graphql") {
    query {
        users {
            id
            name
            email
        }
    }
}
```

### 5. WebSocket Support

Real-time bidirectional communication:

```displexity
websocket("wss://echo.websocket.org") {
    on_connect() {
        print("Connected!")
        send("Hello WebSocket")
    }
    
    on_message(msg) {
        print("Received: " + msg)
    }
    
    on_close() {
        print("Disconnected")
    }
}
```

### 6. JSON Support

Native JSON handling:

```displexity
// Create JSON
json data = {
    "name": "John",
    "age": 30,
    "emails": ["john@example.com", "j.doe@example.com"]
}

// Access JSON
string name = data["name"]
int age = data["age"]
string first_email = data["emails"][0]

// Convert to string
string json_str = data.toString()
```

### 7. Async/Await

Non-blocking operations:

```displexity
async func fetchData(string url) {
    let response = await fetch(url)
    return response.body
}

// Usage
async {
    let data = await fetchData("https://api.example.com/data")
    print("Data: " + data)
}
```

### 8. Error Handling (Try/Catch)

Robust error handling:

```displexity
try {
    // Code that might fail
    let result = divide(10, 0)
} catch (error) {
    print("Error: " + error.message)
    print("Code: " + error.code)
} finally {
    print("Cleanup code")
}

// Custom error throwing
func validate(int age) {
    if (age < 0) {
        throw "Age cannot be negative"
    }
    return true
}
```

### 9. Server Support

Create simple HTTP servers:

```displexity
server(8080) {
    route("GET", "/") {
        return "Hello World"
    }
    
    route("GET", "/api/users/:id") {
        string id = param("id")
        return json {
            "id": id,
            "name": "User " + id
        }
    }
    
    route("POST", "/api/users") {
        let user = request.body.parse_json()
        // Save user...
        return {
            "status": "created",
            "user": user
        }
    }
    
    start()
}
```

### 10. Type Annotations & Generics

Better type safety:

```displexity
// Optional types
func getUser(int? id) -> User? {
    if (id == null) return null
    // ...
}

// Generic types
class List<T> {
    func push(T item) { }
    func pop() -> T { }
}

// Union types
func process(int | string | float value) {
    if (typeof(value) == "int") {
        // ...
    }
}
```

### 11. Testing Framework

Built-in testing:

```displexity
test("addition works") {
    assert(2 + 2 == 4)
    assert(5 + 5 == 10)
}

test("string concatenation") {
    string result = "Hello" + " " + "World"
    assert(result == "Hello World")
}

bench("array creation", 1000) {
    arr = Array[1, 2, 3, 4, 5]
}
```

### 12. Unsafe Code

Low-level memory operations when needed:

```displexity
unsafe {
    // Direct pointer manipulation
    let ptr = alloc(1024)  // Allocate 1KB
    write_byte(ptr, 0, 42)
    int value = read_byte(ptr, 0)
    free(ptr)
}
```

## File Types Reference

| Extension | Purpose | Example |
|-----------|---------|---------|
| `.dis` | Source code | `main.dis`, `module.dis` |
| `.dish` | Header/Interface | `audio_system.dish` |
| `.disll` | Library | `tui_enhanced.disll` |
| `.disp` | Compiled executable | `game.disp` |
| `.dismod` | Module metadata | `module.dismod` |
| `.dispkg` | Package | `lib.dispkg` |

## FFI Type Mapping

### Displexity → C Type Mapping

```
displexity  →  C type
int         →  int / int32_t
float       →  double
string      →  const char*
bool        →  int (0/1)
Array<T>    →  T* (pointer)
void        →  void
```

## Library Declarations

Declare and use external libraries:

```displexity
library("libcurl") {
    extern func curl_easy_init() -> void*
    extern func curl_easy_perform(void* handle) -> int
    extern func curl_easy_cleanup(void* handle) -> void
}

library("libsqlite3") {
    extern func sqlite3_open(string filename, void** ppDb) -> int
    extern func sqlite3_close(void* db) -> int
}
```

## Module Metadata (.dismod)

```json
{
    "name": "mymodule",
    "version": "1.0.0",
    "description": "My awesome module",
    "author": "John Doe",
    "license": "MIT",
    "exports": [
        "add",
        "multiply",
        "divide"
    ],
    "dependencies": [
        {
            "name": "stdlib",
            "version": ">=1.0"
        }
    ]
}
```

## Compilation with FFI

```bash
# Compile with FFI support
disp.exe main.dis -ffi -o main.disp

# Link against external libraries
disp.exe main.dis -libs libm,libcurl,libsqlite3 -o main.disp

# Generate with debug symbols
disp.exe main.dis -debug -o main_debug.disp
```

## Interoperability

### Calling Displexity from C

```c
// Generated C code interface
extern int displexity_main();
extern void* displexity_create_object(const char* type);
extern void displexity_call_method(void* obj, const char* method, ...);
```

### Embedding Displexity in C/C++

```c
#include "displexity_runtime.h"

int main() {
    // Initialize Displexity runtime
    dis_runtime_init();
    
    // Load and run Displexity code
    dis_module* mod = dis_module_load("mygame", "game.disp");
    
    // Call Displexity functions from C
    dis_call_function(mod, "game_init", NULL);
    
    // Cleanup
    dis_module_unload(mod);
    dis_runtime_cleanup();
    
    return 0;
}
```

## Performance Considerations

1. **FFI Calls** - Fast (direct C calls), minimal overhead
2. **Module Loading** - Lazy loading supported
3. **Async/Await** - Non-blocking operations
4. **Garbage Collection** - Automatic for strings and arrays
5. **Optimization** - `-optimize 3` enables aggressive optimization

## Security Features

1. **Sandboxing** - Optional sandboxed execution
2. **Type Safety** - Compile-time type checking
3. **Memory Safety** - Automatic bounds checking
4. **String Safety** - No buffer overflows
5. **FFI Boundaries** - Automatic marshaling

## Examples

### Web Scraper

```displexity
import("http")

func scrape(string url) {
    rest("GET", url) {
        headers {
            "User-Agent": "Mozilla/5.0"
        }
    } then (response) {
        print("Status: " + response.status)
        parse_html(response.body)
    }
}
```

### Database Interface

```displexity
library("libsqlite3")

class Database {
    void* handle
    
    func open(string filename) {
        // Use FFI to call sqlite3_open
    }
    
    func query(string sql) {
        // Execute SQL query
    }
}
```

### Real-time Game Server

```displexity
server(9000) {
    websocket("/game") {
        on_message(player_id, msg) {
            // Broadcast to all players
            broadcast(msg)
        }
    }
    
    route("POST", "/api/join") {
        let player_id = create_player()
        return {
            "player_id": player_id,
            "server": "game.example.com:9000"
        }
    }
}
```

## Future Enhancements

- [ ] Rust FFI support
- [ ] Python bindings
- [ ] .NET interop
- [ ] JNI for Java
- [ ] More test frameworks
- [ ] Performance profiler
- [ ] IDE plugins
- [ ] Package manager

---

## Next Steps

1. **Build** - `disp.exe main.dis -ffi -o app.disp`
2. **Link** - Include required external libraries
3. **Test** - Run unit tests with `test()` blocks
4. **Deploy** - Distribute the `.disp` executable

All these features are integrated into the Displexity compiler and runtime!
