#ifndef DISPLEXITY_CODEGEN_H
#define DISPLEXITY_CODEGEN_H

#include "ast.h"
#include <sstream>
#include <map>
#include <set>

namespace disp {

class CodeGenerator {
private:
    std::stringstream code;
    std::map<std::string, std::string> variables;
    std::set<std::string> declaredVars;
    std::map<std::string, std::string> varTypes;  // variable name -> type (int, float, str, bool)
    int tmpCounter = 0;
    bool needsGraphics = false;
    bool needsHttp = false;
    bool needsKeypress = false;
    bool needsAI = false;
    bool needsImage = false;
    bool needsSound = false;
    bool needsTUI = false;
    std::string appIconPath;  // Icon path set by for_this_use_icon()

    std::string genTmp() {
        return "__tmp_" + std::to_string(tmpCounter++);
    }

public:
    // Get the icon path set by for_this_use_icon()
    std::string getAppIconPath() const { return appIconPath; }

public:
    std::string generate(const Program& prog) {
        code.str("");
        appIconPath = "";  // Reset icon path
        
        // Scan for required features
        scanFeatures(prog);
        
        // Emit includes
        code << "#include <stdio.h>\n";
        code << "#include <string.h>\n";
        code << "#include <stdlib.h>\n";
        
        if (needsKeypress) {
            code << "#ifdef _WIN32\n";
            code << "#include <conio.h>\n";
            code << "#include <windows.h>\n";
            code << "#else\n";
            code << "#include <termios.h>\n";
            code << "#include <unistd.h>\n";
            code << "#endif\n";
        }
        
        if (needsGraphics) {
            code << "#ifdef _WIN32\n";
            code << "#include <windows.h>\n";
            code << "#endif\n";
        }
        
        if (needsHttp) {
            code << "#ifdef _WIN32\n";
            code << "#include <windows.h>\n";
            code << "#include <wininet.h>\n";
            code << "#pragma comment(lib, \"wininet.lib\")\n";
            code << "#else\n";
            code << "#include <curl/curl.h>\n";
            code << "#endif\n";
        }
        
        code << "\n";
        
        // Emit runtime helpers
        emitRuntimeHelpers();
        
        // Pre-declare functions
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                code << func->returnType << " " << func->name << "(";
                for (size_t i = 0; i < func->params.size(); i++) {
                    std::string paramType = func->params[i].second;
                    if (paramType == "str") {
                        code << "const char* " << func->params[i].first;
                    } else {
                        code << paramType << " " << func->params[i].first;
                    }
                    if (i < func->params.size() - 1) code << ", ";
                }
                code << ");\n";
            }
        }
        code << "\n";

        // Check if user defined a main function
        bool hasUserMain = false;
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                if (func->name == "main") {
                    hasUserMain = true;
                    break;
                }
            }
        }

        // Emit functions
        for (const auto& stmt : prog.statements) {
            if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                emitFunction(func);
            }
        }

        // Only generate main wrapper if user didn't define main
        if (!hasUserMain) {
            code << "\nint main(int argc, char** argv) {\n";
            if (needsGraphics) {
                code << "    __disp_graphics_init();\n";
            }
            for (const auto& stmt : prog.statements) {
                if (!std::dynamic_pointer_cast<FunctionDecl>(stmt) &&
                    !std::dynamic_pointer_cast<ClassDecl>(stmt)) {
                    emitStatement(stmt);
                }
            }
            if (needsGraphics) {
                code << "    __disp_graphics_cleanup();\n";
            }
            code << "    return 0;\n";
            code << "}\n";
        }

        return code.str();
    }

private:
    void scanFeatures(const Program& prog) {
        for (const auto& stmt : prog.statements) {
            scanStatement(stmt);
        }
    }
    
    void scanStatement(const StmtPtr& stmt) {
        if (std::dynamic_pointer_cast<GraphicsStmt>(stmt)) needsGraphics = true;
        else if (std::dynamic_pointer_cast<HttpStmt>(stmt)) needsHttp = true;
        else if (std::dynamic_pointer_cast<KeypressStmt>(stmt)) needsKeypress = true;
        else if (std::dynamic_pointer_cast<HotkeyStmt>(stmt)) { needsKeypress = true; }
        else if (std::dynamic_pointer_cast<AIStmt>(stmt)) { needsAI = true; needsHttp = true; }
        else if (std::dynamic_pointer_cast<InputStmt>(stmt)) { /* input always available */ }
        else if (auto iconStmt = std::dynamic_pointer_cast<IconStmt>(stmt)) {
            appIconPath = iconStmt->iconPath;
        }
        else if (auto func = std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
            for (const auto& s : func->body) scanStatement(s);
        }
        else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            for (const auto& s : ifStmt->thenBlock) scanStatement(s);
            for (const auto& s : ifStmt->elseBlock) scanStatement(s);
            scanExpression(ifStmt->condition);
        }
        else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            for (const auto& s : whileStmt->body) scanStatement(s);
            scanExpression(whileStmt->condition);
        }
        else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            for (const auto& s : forStmt->body) scanStatement(s);
            scanExpression(forStmt->start);
            scanExpression(forStmt->end);
        }
        else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            scanExpression(assign->value);
        }
        else if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(stmt)) {
            if (varDecl->initialValue) {
                scanExpression(varDecl->initialValue);
            }
        }
        else if (auto print = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            scanExpression(print->expr);
        }
    }
    
    void scanExpression(const ExprPtr& expr) {
        if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
            // Check for input functions
            if (funcCall->name == "is_key_pressed" || funcCall->name == "is_key_just_pressed" || 
                funcCall->name == "is_key_released" || funcCall->name == "get_mouse_x" || 
                funcCall->name == "get_mouse_y" || funcCall->name == "is_mouse_pressed" || 
                funcCall->name == "is_mouse_just_pressed" || funcCall->name == "is_mouse_released" || 
                funcCall->name == "get_mouse_wheel" || funcCall->name == "set_mouse_position" || 
                funcCall->name == "show_cursor") {
                needsKeypress = true;
            }
            // Check for image/sound functions
            if (funcCall->name == "load_image" || funcCall->name == "draw_image") {
                needsImage = true;
                needsGraphics = true;
            }
            if (funcCall->name == "load_sound" || funcCall->name == "play_sound" || 
                funcCall->name == "stop_sound" || funcCall->name == "pause_sound") {
                needsSound = true;
            }
            // Scan function arguments
            for (const auto& arg : funcCall->args) {
                scanExpression(arg);
            }
        }
        else if (auto loadImg = std::dynamic_pointer_cast<LoadImageExpr>(expr)) {
            needsImage = true;
            needsGraphics = true;
        }
        else if (auto sndExpr = std::dynamic_pointer_cast<SoundExpr>(expr)) {
            needsSound = true;
        }
        else if (auto binOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
            scanExpression(binOp->left);
            scanExpression(binOp->right);
        }
        else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
            scanExpression(unaryOp->expr);
        }
    }
    
    void emitRuntimeHelpers() {
        // Input sensing helper with comprehensive keyboard and mouse support
        if (needsKeypress) {
            code << "// Comprehensive Input System\n";
            code << "#ifdef _WIN32\n";
            code << "#include <windows.h>\n";
            code << "static POINT __disp_mouse_pos = {0, 0};\n";
            code << "static bool __disp_keys[256] = {false};\n";
            code << "static bool __disp_mouse_buttons[8] = {false};\n";
            code << "static int __disp_mouse_wheel = 0;\n";
            code << "static HWND __disp_input_hwnd = NULL;\n\n";
            
            // Update input state
            code << "void __disp_update_input() {\n";
            code << "    // Update mouse position\n";
            code << "    GetCursorPos(&__disp_mouse_pos);\n";
            code << "    if (__disp_input_hwnd) ScreenToClient(__disp_input_hwnd, &__disp_mouse_pos);\n";
            code << "    \n";
            code << "    // Update key states\n";
            code << "    for (int i = 0; i < 256; i++) {\n";
            code << "        __disp_keys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;\n";
            code << "    }\n";
            code << "    \n";
            code << "    // Update mouse buttons\n";
            code << "    __disp_mouse_buttons[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;\n";
            code << "    __disp_mouse_buttons[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;\n";
            code << "    __disp_mouse_buttons[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;\n";
            code << "    __disp_mouse_buttons[3] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;\n";
            code << "    __disp_mouse_buttons[4] = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;\n";
            code << "}\n\n";
            
            // Key sensing functions
            code << "int __disp_is_key_pressed(int key_code) {\n";
            code << "    __disp_update_input();\n";
            code << "    return __disp_keys[key_code] ? 1 : 0;\n";
            code << "}\n\n";
            
            code << "int __disp_is_key_just_pressed(int key_code) {\n";
            code << "    static bool prev_keys[256] = {false};\n";
            code << "    __disp_update_input();\n";
            code << "    bool result = __disp_keys[key_code] && !prev_keys[key_code];\n";
            code << "    prev_keys[key_code] = __disp_keys[key_code];\n";
            code << "    return result ? 1 : 0;\n";
            code << "}\n\n";
            
            code << "int __disp_is_key_released(int key_code) {\n";
            code << "    static bool prev_keys[256] = {false};\n";
            code << "    __disp_update_input();\n";
            code << "    bool result = !__disp_keys[key_code] && prev_keys[key_code];\n";
            code << "    prev_keys[key_code] = __disp_keys[key_code];\n";
            code << "    return result ? 1 : 0;\n";
            code << "}\n\n";
            
            // Mouse sensing functions
            code << "int __disp_get_mouse_x() {\n";
            code << "    __disp_update_input();\n";
            code << "    return __disp_mouse_pos.x;\n";
            code << "}\n\n";
            
            code << "int __disp_get_mouse_y() {\n";
            code << "    __disp_update_input();\n";
            code << "    return __disp_mouse_pos.y;\n";
            code << "}\n\n";
            
            code << "int __disp_is_mouse_pressed(int button) {\n";
            code << "    __disp_update_input();\n";
            code << "    return (button >= 0 && button < 8) ? __disp_mouse_buttons[button] : 0;\n";
            code << "}\n\n";
            
            code << "int __disp_is_mouse_just_pressed(int button) {\n";
            code << "    static bool prev_mouse[8] = {false};\n";
            code << "    __disp_update_input();\n";
            code << "    if (button < 0 || button >= 8) return 0;\n";
            code << "    bool result = __disp_mouse_buttons[button] && !prev_mouse[button];\n";
            code << "    prev_mouse[button] = __disp_mouse_buttons[button];\n";
            code << "    return result ? 1 : 0;\n";
            code << "}\n\n";
            
            code << "int __disp_is_mouse_released(int button) {\n";
            code << "    static bool prev_mouse[8] = {false};\n";
            code << "    __disp_update_input();\n";
            code << "    if (button < 0 || button >= 8) return 0;\n";
            code << "    bool result = !__disp_mouse_buttons[button] && prev_mouse[button];\n";
            code << "    prev_mouse[button] = __disp_mouse_buttons[button];\n";
            code << "    return result ? 1 : 0;\n";
            code << "}\n\n";
            
            code << "void __disp_set_mouse_position(int x, int y) {\n";
            code << "    if (__disp_input_hwnd) {\n";
            code << "        POINT pt = {x, y};\n";
            code << "        ClientToScreen(__disp_input_hwnd, &pt);\n";
            code << "        SetCursorPos(pt.x, pt.y);\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_show_cursor(int show) {\n";
            code << "    ShowCursor(show ? TRUE : FALSE);\n";
            code << "}\n\n";
            
            code << "int __disp_get_mouse_wheel() {\n";
            code << "    int result = __disp_mouse_wheel;\n";
            code << "    __disp_mouse_wheel = 0;  // Reset after reading\n";
            code << "    return result;\n";
            code << "}\n\n";
            
            // Key constants
            code << "// Key code constants\n";
            code << "#define DISP_KEY_SPACE 32\n";
            code << "#define DISP_KEY_ENTER 13\n";
            code << "#define DISP_KEY_ESC 27\n";
            code << "#define DISP_KEY_TAB 9\n";
            code << "#define DISP_KEY_BACKSPACE 8\n";
            code << "#define DISP_KEY_DELETE 46\n";
            code << "#define DISP_KEY_LEFT 37\n";
            code << "#define DISP_KEY_UP 38\n";
            code << "#define DISP_KEY_RIGHT 39\n";
            code << "#define DISP_KEY_DOWN 40\n";
            code << "#define DISP_KEY_SHIFT 16\n";
            code << "#define DISP_KEY_CTRL 17\n";
            code << "#define DISP_KEY_ALT 18\n";
            code << "#define DISP_KEY_F1 112\n";
            code << "#define DISP_KEY_F2 113\n";
            code << "#define DISP_KEY_F3 114\n";
            code << "#define DISP_KEY_F4 115\n";
            code << "#define DISP_KEY_F5 116\n";
            code << "#define DISP_KEY_F6 117\n";
            code << "#define DISP_KEY_F7 118\n";
            code << "#define DISP_KEY_F8 119\n";
            code << "#define DISP_KEY_F9 120\n";
            code << "#define DISP_KEY_F10 121\n";
            code << "#define DISP_KEY_F11 122\n";
            code << "#define DISP_KEY_F12 123\n";
            code << "#define DISP_MOUSE_LEFT 0\n";
            code << "#define DISP_MOUSE_RIGHT 1\n";
            code << "#define DISP_MOUSE_MIDDLE 2\n";
            code << "#define DISP_MOUSE_X1 3\n";
            code << "#define DISP_MOUSE_X2 4\n\n";
            
            code << "#else\n";
            code << "// Linux input stubs\n";
            code << "void __disp_update_input() {}\n";
            code << "int __disp_is_key_pressed(int key_code) { return 0; }\n";
            code << "int __disp_is_key_just_pressed(int key_code) { return 0; }\n";
            code << "int __disp_is_key_released(int key_code) { return 0; }\n";
            code << "int __disp_get_mouse_x() { return 0; }\n";
            code << "int __disp_get_mouse_y() { return 0; }\n";
            code << "int __disp_is_mouse_pressed(int button) { return 0; }\n";
            code << "int __disp_is_mouse_just_pressed(int button) { return 0; }\n";
            code << "int __disp_is_mouse_released(int button) { return 0; }\n";
            code << "void __disp_set_mouse_position(int x, int y) {}\n";
            code << "void __disp_show_cursor(int show) {}\n";
            code << "int __disp_get_mouse_wheel() { return 0; }\n";
            code << "#endif\n\n";
            
            // Original keypress function for compatibility
            code << "int __disp_get_keypress() {\n";
            code << "#ifdef _WIN32\n";
            code << "    return _getch();\n";
            code << "#else\n";
            code << "    struct termios oldt, newt;\n";
            code << "    int ch;\n";
            code << "    tcgetattr(STDIN_FILENO, &oldt);\n";
            code << "    newt = oldt;\n";
            code << "    newt.c_lflag &= ~(ICANON | ECHO);\n";
            code << "    tcsetattr(STDIN_FILENO, TCSANOW, &newt);\n";
            code << "    ch = getchar();\n";
            code << "    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);\n";
            code << "    return ch;\n";
            code << "#endif\n";
            code << "}\n\n";
            
            code << "char* __disp_keypress_to_string(int key) {\n";
            code << "    static char buf[2] = {0, 0};\n";
            code << "    buf[0] = (char)key;\n";
            code << "    return buf;\n";
            code << "}\n\n";
            
            // Hotkey system
            code << "typedef void (*__disp_hotkey_fn)();\n";
            code << "struct { int key; __disp_hotkey_fn fn; } __disp_hotkeys[256];\n";
            code << "int __disp_hotkey_count = 0;\n\n";
            code << "void __disp_register_hotkey(int key, __disp_hotkey_fn fn) {\n";
            code << "    __disp_hotkeys[__disp_hotkey_count].key = key;\n";
            code << "    __disp_hotkeys[__disp_hotkey_count].fn = fn;\n";
            code << "    __disp_hotkey_count++;\n";
            code << "}\n\n";
        }
        
        // HTTP helper
        if (needsHttp) {
            code << "// HTTP runtime\n";
            code << "#ifdef _WIN32\n";
            code << "char* __disp_http_get(const char* url) {\n";
            code << "    static char buffer[65536];\n";
            code << "    HINTERNET hInternet = InternetOpenA(\"DISP\", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);\n";
            code << "    if (!hInternet) return \"\";\n";
            code << "    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);\n";
            code << "    if (!hUrl) { InternetCloseHandle(hInternet); return \"\"; }\n";
            code << "    DWORD bytesRead = 0;\n";
            code << "    InternetReadFile(hUrl, buffer, sizeof(buffer)-1, &bytesRead);\n";
            code << "    buffer[bytesRead] = 0;\n";
            code << "    InternetCloseHandle(hUrl);\n";
            code << "    InternetCloseHandle(hInternet);\n";
            code << "    return buffer;\n";
            code << "}\n";
            code << "#else\n";
            code << "static size_t __curl_write(void* p, size_t s, size_t n, void* u) {\n";
            code << "    strncat((char*)u, (char*)p, s*n);\n";
            code << "    return s*n;\n";
            code << "}\n";
            code << "char* __disp_http_get(const char* url) {\n";
            code << "    static char buffer[65536] = {0};\n";
            code << "    CURL* curl = curl_easy_init();\n";
            code << "    if (curl) {\n";
            code << "        curl_easy_setopt(curl, CURLOPT_URL, url);\n";
            code << "        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __curl_write);\n";
            code << "        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);\n";
            code << "        curl_easy_perform(curl);\n";
            code << "        curl_easy_cleanup(curl);\n";
            code << "    }\n";
            code << "    return buffer;\n";
            code << "}\n";
            code << "#endif\n\n";
        }
        
        // Graphics helper with extended OpenGL support
        if (needsGraphics) {
            code << "// Graphics runtime with comprehensive OpenGL support\n";
            code << "#ifdef _WIN32\n";
            code << "#include <gl/gl.h>\n";
            code << "#include <gl/glu.h>\n";
            code << "#include <math.h>\n";
            code << "#pragma comment(lib, \"opengl32.lib\")\n";
            code << "#pragma comment(lib, \"glu32.lib\")\n";
            code << "HWND __disp_hwnd = NULL;\n";
            code << "HDC __disp_hdc = NULL;\n";
            code << "HGLRC __disp_hglrc = NULL;\n";
            code << "int __disp_width = 800, __disp_height = 600;\n";
            code << "int __disp_gl_enabled = 0;\n\n";
            
            // Extended OpenGL functions
            code << "// Extended OpenGL API\n";
            code << "void __disp_gl_enable(int cap) { if (__disp_gl_enabled) glEnable(cap); }\n";
            code << "void __disp_gl_disable(int cap) { if (__disp_gl_enabled) glDisable(cap); }\n";
            code << "void __disp_gl_blend_func(int sfactor, int dfactor) { if (__disp_gl_enabled) glBlendFunc(sfactor, dfactor); }\n";
            code << "void __disp_gl_depth_func(int func) { if (__disp_gl_enabled) glDepthFunc(func); }\n";
            code << "void __disp_gl_cull_face(int mode) { if (__disp_gl_enabled) glCullFace(mode); }\n";
            code << "void __disp_gl_polygon_mode(int face, int mode) { if (__disp_gl_enabled) glPolygonMode(face, mode); }\n";
            code << "void __disp_gl_line_width(float width) { if (__disp_gl_enabled) glLineWidth(width); }\n";
            code << "void __disp_gl_point_size(float size) { if (__disp_gl_enabled) glPointSize(size); }\n";
            
            // Texture functions
            code << "void __disp_gl_gen_textures(int n, unsigned int* textures) { if (__disp_gl_enabled) glGenTextures(n, textures); }\n";
            code << "void __disp_gl_bind_texture(int target, unsigned int texture) { if (__disp_gl_enabled) glBindTexture(target, texture); }\n";
            code << "void __disp_gl_tex_parameter_i(int target, int pname, int param) { if (__disp_gl_enabled) glTexParameteri(target, pname, param); }\n";
            
            // Buffer functions (commented out - not available in basic OpenGL)
            // code << "void __disp_gl_gen_buffers(int n, unsigned int* buffers) { if (__disp_gl_enabled) glGenBuffers(n, buffers); }\n";
            // code << "void __disp_gl_bind_buffer(int target, unsigned int buffer) { if (__disp_gl_enabled) glBindBuffer(target, buffer); }\n";
            // code << "void __disp_gl_buffer_data(int target, int size, const void* data, int usage) { if (__disp_gl_enabled) glBufferData(target, size, data, usage); }\n";
            
            // Shader functions (commented out - not available in basic OpenGL)
            // code << "unsigned int __disp_gl_create_shader(int type) { return __disp_gl_enabled ? glCreateShader(type) : 0; }\n";
            // code << "void __disp_gl_shader_source(unsigned int shader, int count, const char** string, const int* length) { if (__disp_gl_enabled) glShaderSource(shader, count, string, length); }\n";
            // code << "void __disp_gl_compile_shader(unsigned int shader) { if (__disp_gl_enabled) glCompileShader(shader); }\n";
            // code << "unsigned int __disp_gl_create_program() { return __disp_gl_enabled ? glCreateProgram() : 0; }\n";
            // code << "void __disp_gl_attach_shader(unsigned int program, unsigned int shader) { if (__disp_gl_enabled) glAttachShader(program, shader); }\n";
            // code << "void __disp_gl_link_program(unsigned int program) { if (__disp_gl_enabled) glLinkProgram(program); }\n";
            // code << "void __disp_gl_use_program(unsigned int program) { if (__disp_gl_enabled) glUseProgram(program); }\n";
            
            code << "LRESULT CALLBACK __disp_wndproc(HWND h, UINT m, WPARAM w, LPARAM l) {\n";
            code << "    switch(m) {\n";
            code << "        case WM_DESTROY: PostQuitMessage(0); return 0;\n";
            code << "        case WM_SIZE: __disp_width = LOWORD(l); __disp_height = HIWORD(l);\n";
            code << "                      if (__disp_gl_enabled) glViewport(0, 0, __disp_width, __disp_height); break;\n";
            code << "        case WM_KEYDOWN: /* Handle key events */ break;\n";
            code << "    }\n";
            code << "    return DefWindowProcA(h, m, w, l);\n";
            code << "}\n\n";
            
            code << "void __disp_graphics_init() {\n";
            code << "    WNDCLASSA wc = {0};\n";
            code << "    wc.lpfnWndProc = __disp_wndproc;\n";
            code << "    wc.hInstance = GetModuleHandle(NULL);\n";
            code << "    wc.lpszClassName = \"DISP\";\n";
            code << "    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);\n";
            code << "    wc.hCursor = LoadCursor(NULL, IDC_ARROW);\n";
            code << "    RegisterClassA(&wc);\n";
            code << "}\n\n";
            
            code << "void __disp_window_create(const char* title, int w, int h) {\n";
            code << "    __disp_width = w; __disp_height = h;\n";
            code << "    __disp_hwnd = CreateWindowA(\"DISP\", title, WS_OVERLAPPEDWINDOW|WS_VISIBLE,\n";
            code << "        CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, GetModuleHandle(NULL), NULL);\n";
            code << "    __disp_hdc = GetDC(__disp_hwnd);\n";
            code << "}\n\n";
            
            code << "void __disp_gl_init() {\n";
            code << "    if (!__disp_hdc) return;\n";
            code << "    PIXELFORMATDESCRIPTOR pfd = {\n";
            code << "        sizeof(PIXELFORMATDESCRIPTOR), 1,\n";
            code << "        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,\n";
            code << "        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\n";
            code << "        24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0\n";
            code << "    };\n";
            code << "    int format = ChoosePixelFormat(__disp_hdc, &pfd);\n";
            code << "    SetPixelFormat(__disp_hdc, format, &pfd);\n";
            code << "    __disp_hglrc = wglCreateContext(__disp_hdc);\n";
            code << "    wglMakeCurrent(__disp_hdc, __disp_hglrc);\n";
            code << "    __disp_gl_enabled = 1;\n";
            code << "    glViewport(0, 0, __disp_width, __disp_height);\n";
            code << "    glEnable(GL_DEPTH_TEST);\n";
            code << "}\n\n";
            
            code << "void __disp_clear() {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);\n";
            code << "    } else if (__disp_hdc) {\n";
            code << "        RECT r = {0, 0, __disp_width, __disp_height};\n";
            code << "        FillRect(__disp_hdc, &r, (HBRUSH)(COLOR_WINDOW+1));\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_set_clear_color(float r, float g, float b, float a) {\n";
            code << "    if (__disp_gl_enabled) glClearColor(r, g, b, a);\n";
            code << "}\n\n";
            
            code << "void __disp_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_TRIANGLES);\n";
            code << "        glVertex2f(x1, y1); glVertex2f(x2, y2); glVertex2f(x3, y3);\n";
            code << "        glEnd();\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_draw_quad(float x, float y, float w, float h) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_QUADS);\n";
            code << "        glVertex2f(x, y); glVertex2f(x+w, y);\n";
            code << "        glVertex2f(x+w, y+h); glVertex2f(x, y+h);\n";
            code << "        glEnd();\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_set_color(float r, float g, float b) {\n";
            code << "    if (__disp_gl_enabled) glColor3f(r, g, b);\n";
            code << "}\n\n";
            
            code << "void __disp_draw_pixel(int x, int y) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_POINTS);\n";
            code << "        glVertex2i(x, y);\n";
            code << "        glEnd();\n";
            code << "    } else if (__disp_hdc) {\n";
            code << "        SetPixel(__disp_hdc, x, y, RGB(0,0,0));\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_draw_line(int x1, int y1, int x2, int y2) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_LINES);\n";
            code << "        glVertex2i(x1, y1); glVertex2i(x2, y2);\n";
            code << "        glEnd();\n";
            code << "    } else if (__disp_hdc) {\n";
            code << "        MoveToEx(__disp_hdc, x1, y1, NULL); LineTo(__disp_hdc, x2, y2);\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_draw_rect(int x, int y, int w, int h) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_LINE_LOOP);\n";
            code << "        glVertex2i(x, y); glVertex2i(x+w, y);\n";
            code << "        glVertex2i(x+w, y+h); glVertex2i(x, y+h);\n";
            code << "        glEnd();\n";
            code << "    } else if (__disp_hdc) {\n";
            code << "        Rectangle(__disp_hdc, x, y, x+w, y+h);\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_fill_rect(int x, int y, int w, int h) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_QUADS);\n";
            code << "        glVertex2i(x, y); glVertex2i(x+w, y);\n";
            code << "        glVertex2i(x+w, y+h); glVertex2i(x, y+h);\n";
            code << "        glEnd();\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_draw_circle(int cx, int cy, int radius) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_LINE_LOOP);\n";
            code << "        for (int i = 0; i < 360; i += 5) {\n";
            code << "            float angle = i * 3.14159f / 180.0f;\n";
            code << "            glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);\n";
            code << "        }\n";
            code << "        glEnd();\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_fill_circle(int cx, int cy, int radius) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glBegin(GL_TRIANGLE_FAN);\n";
            code << "        glVertex2i(cx, cy);\n";
            code << "        for (int i = 0; i <= 360; i += 5) {\n";
            code << "            float angle = i * 3.14159f / 180.0f;\n";
            code << "            glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);\n";
            code << "        }\n";
            code << "        glEnd();\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_set_projection_2d() {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glMatrixMode(GL_PROJECTION);\n";
            code << "        glLoadIdentity();\n";
            code << "        glOrtho(0, __disp_width, __disp_height, 0, -1, 1);\n";
            code << "        glMatrixMode(GL_MODELVIEW);\n";
            code << "        glLoadIdentity();\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_set_projection_3d(float fov, float near_plane, float far_plane) {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        glMatrixMode(GL_PROJECTION);\n";
            code << "        glLoadIdentity();\n";
            code << "        float aspect = (float)__disp_width / (float)__disp_height;\n";
            code << "        float f = 1.0f / tan(fov * 3.14159f / 360.0f);\n";
            code << "        glFrustum(-aspect/f, aspect/f, -1.0f/f, 1.0f/f, near_plane, far_plane);\n";
            code << "        glMatrixMode(GL_MODELVIEW);\n";
            code << "        glLoadIdentity();\n";
            code << "    }\n";
            code << "}\n\n";
            
            code << "void __disp_translate(float x, float y, float z) {\n";
            code << "    if (__disp_gl_enabled) glTranslatef(x, y, z);\n";
            code << "}\n\n";
            
            code << "void __disp_rotate(float angle, float x, float y, float z) {\n";
            code << "    if (__disp_gl_enabled) glRotatef(angle, x, y, z);\n";
            code << "}\n\n";
            
            code << "void __disp_scale(float x, float y, float z) {\n";
            code << "    if (__disp_gl_enabled) glScalef(x, y, z);\n";
            code << "}\n\n";
            
            code << "void __disp_push_matrix() {\n";
            code << "    if (__disp_gl_enabled) glPushMatrix();\n";
            code << "}\n\n";
            
            code << "void __disp_pop_matrix() {\n";
            code << "    if (__disp_gl_enabled) glPopMatrix();\n";
            code << "}\n\n";
            
            code << "int __disp_window_should_close() {\n";
            code << "    MSG msg;\n";
            code << "    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {\n";
            code << "        if (msg.message == WM_QUIT) return 1;\n";
            code << "        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {\n";
            code << "            PostQuitMessage(0); return 1;\n";
            code << "        }\n";
            code << "        TranslateMessage(&msg); DispatchMessage(&msg);\n";
            code << "    }\n";
            code << "    return 0;\n";
            code << "}\n\n";
            
            code << "void __disp_render() {\n";
            code << "    if (__disp_gl_enabled) {\n";
            code << "        SwapBuffers(__disp_hdc);\n";
            code << "    }\n";
            code << "    MSG msg;\n";
            code << "    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {\n";
            code << "        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {\n";
            code << "            PostQuitMessage(0);\n";
            code << "        }\n";
            code << "        TranslateMessage(&msg); DispatchMessage(&msg);\n";
            code << "    }\n";
            code << "    Sleep(16); // ~60 FPS\n";
            code << "}\n\n";
            
            code << "void __disp_graphics_cleanup() {\n";
            code << "    if (__disp_hglrc) { wglMakeCurrent(NULL, NULL); wglDeleteContext(__disp_hglrc); }\n";
            code << "    if (__disp_hdc) ReleaseDC(__disp_hwnd, __disp_hdc);\n";
            code << "    if (__disp_hwnd) DestroyWindow(__disp_hwnd);\n";
            code << "}\n";
            
            code << "#else\n";
            code << "// Linux OpenGL support (stub)\n";
            code << "void __disp_graphics_init() { printf(\"Graphics init\\n\"); }\n";
            code << "void __disp_window_create(const char* t, int w, int h) { printf(\"Window: %s %dx%d\\n\", t, w, h); }\n";
            code << "void __disp_gl_init() { printf(\"OpenGL init\\n\"); }\n";
            code << "void __disp_clear() { printf(\"Clear\\n\"); }\n";
            code << "void __disp_set_clear_color(float r, float g, float b, float a) {}\n";
            code << "void __disp_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {}\n";
            code << "void __disp_draw_quad(float x, float y, float w, float h) {}\n";
            code << "void __disp_set_color(float r, float g, float b) {}\n";
            code << "void __disp_draw_pixel(int x, int y) {}\n";
            code << "void __disp_draw_line(int x1, int y1, int x2, int y2) {}\n";
            code << "void __disp_draw_rect(int x, int y, int w, int h) {}\n";
            code << "void __disp_fill_rect(int x, int y, int w, int h) {}\n";
            code << "void __disp_draw_circle(int cx, int cy, int radius) {}\n";
            code << "void __disp_fill_circle(int cx, int cy, int radius) {}\n";
            code << "void __disp_set_projection_2d() {}\n";
            code << "void __disp_set_projection_3d(float fov, float near_plane, float far_plane) {}\n";
            code << "void __disp_translate(float x, float y, float z) {}\n";
            code << "void __disp_rotate(float angle, float x, float y, float z) {}\n";
            code << "void __disp_scale(float x, float y, float z) {}\n";
            code << "void __disp_push_matrix() {}\n";
            code << "void __disp_pop_matrix() {}\n";
            code << "int __disp_window_should_close() { return 0; }\n";
            code << "void __disp_render() {}\n";
            code << "void __disp_graphics_cleanup() {}\n";
            code << "#endif\n\n";
        }
        
        // AI helper
        if (needsAI) {
            code << "// AI runtime (stub - requires API key)\n";
            code << "char* __disp_ai_query(const char* prompt) {\n";
            code << "    static char response[1024];\n";
            code << "    snprintf(response, sizeof(response), \"AI Response to: %s\", prompt);\n";
            code << "    return response;\n";
            code << "}\n\n";
        }
        
        // Image loading helper (like ctx.drawImage with 9 args)
        if (needsImage) {
            code << "// Image loading runtime\n";
            code << "#ifdef _WIN32\n";
            code << "#include <wingdi.h>\n";
            code << "typedef struct { int width, height; unsigned char* data; HBITMAP hBitmap; } __disp_image;\n";
            code << "__disp_image __disp_images[256];\n";
            code << "int __disp_image_count = 0;\n\n";
            
            code << "int __disp_load_image(const char* path) {\n";
            code << "    if (__disp_image_count >= 256) return -1;\n";
            code << "    int id = __disp_image_count++;\n";
            code << "    // Load BMP image (simplified)\n";
            code << "    __disp_images[id].hBitmap = (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);\n";
            code << "    if (__disp_images[id].hBitmap) {\n";
            code << "        BITMAP bm;\n";
            code << "        GetObject(__disp_images[id].hBitmap, sizeof(bm), &bm);\n";
            code << "        __disp_images[id].width = bm.bmWidth;\n";
            code << "        __disp_images[id].height = bm.bmHeight;\n";
            code << "    }\n";
            code << "    return id;\n";
            code << "}\n\n";
            
            // draw_image with 9 args like ctx.drawImage(img, sx, sy, sw, sh, dx, dy, dw, dh)
            code << "void __disp_draw_image_9(int id, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {\n";
            code << "    if (id < 0 || id >= __disp_image_count) return;\n";
            code << "    if (!__disp_hdc || !__disp_images[id].hBitmap) return;\n";
            code << "    HDC memDC = CreateCompatibleDC(__disp_hdc);\n";
            code << "    SelectObject(memDC, __disp_images[id].hBitmap);\n";
            code << "    StretchBlt(__disp_hdc, dx, dy, dw, dh, memDC, sx, sy, sw, sh, SRCCOPY);\n";
            code << "    DeleteDC(memDC);\n";
            code << "}\n\n";
            
            // Simplified draw_image with fewer args
            code << "void __disp_draw_image(int id, int x, int y) {\n";
            code << "    if (id < 0 || id >= __disp_image_count) return;\n";
            code << "    __disp_draw_image_9(id, 0, 0, __disp_images[id].width, __disp_images[id].height, x, y, __disp_images[id].width, __disp_images[id].height);\n";
            code << "}\n\n";
            
            code << "void __disp_draw_image_scaled(int id, int x, int y, int w, int h) {\n";
            code << "    if (id < 0 || id >= __disp_image_count) return;\n";
            code << "    __disp_draw_image_9(id, 0, 0, __disp_images[id].width, __disp_images[id].height, x, y, w, h);\n";
            code << "}\n\n";
            
            code << "#else\n";
            code << "// Linux image stubs\n";
            code << "int __disp_load_image(const char* path) { return -1; }\n";
            code << "void __disp_draw_image_9(int id, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {}\n";
            code << "void __disp_draw_image(int id, int x, int y) {}\n";
            code << "void __disp_draw_image_scaled(int id, int x, int y, int w, int h) {}\n";
            code << "#endif\n\n";
        }
        
        // Sound loading helper
        if (needsSound) {
            code << "// Sound/Audio runtime\n";
            code << "#ifdef _WIN32\n";
            code << "#include <mmsystem.h>\n";
            code << "#pragma comment(lib, \"winmm.lib\")\n";
            code << "typedef struct { char path[256]; int playing; } __disp_sound;\n";
            code << "__disp_sound __disp_sounds[64];\n";
            code << "int __disp_sound_count = 0;\n\n";
            
            code << "int __disp_load_sound(const char* path) {\n";
            code << "    if (__disp_sound_count >= 64) return -1;\n";
            code << "    int id = __disp_sound_count++;\n";
            code << "    strncpy(__disp_sounds[id].path, path, 255);\n";
            code << "    __disp_sounds[id].playing = 0;\n";
            code << "    return id;\n";
            code << "}\n\n";
            
            code << "void __disp_play_sound(int id, int loop) {\n";
            code << "    if (id < 0 || id >= __disp_sound_count) return;\n";
            code << "    DWORD flags = SND_FILENAME | SND_ASYNC;\n";
            code << "    if (loop) flags |= SND_LOOP;\n";
            code << "    PlaySoundA(__disp_sounds[id].path, NULL, flags);\n";
            code << "    __disp_sounds[id].playing = 1;\n";
            code << "}\n\n";
            
            code << "void __disp_stop_sound(int id) {\n";
            code << "    if (id < 0 || id >= __disp_sound_count) return;\n";
            code << "    PlaySoundA(NULL, NULL, 0);\n";
            code << "    __disp_sounds[id].playing = 0;\n";
            code << "}\n\n";
            
            code << "void __disp_pause_sound(int id) {\n";
            code << "    __disp_stop_sound(id);\n";
            code << "}\n\n";
            
            code << "#else\n";
            code << "// Linux sound stubs\n";
            code << "int __disp_load_sound(const char* path) { return -1; }\n";
            code << "void __disp_play_sound(int id, int loop) {}\n";
            code << "void __disp_stop_sound(int id) {}\n";
            code << "void __disp_pause_sound(int id) {}\n";
            code << "#endif\n\n";
        }
    }

    void emitFunction(std::shared_ptr<FunctionDecl> func) {
        code << func->returnType << " " << func->name << "(";
        for (size_t i = 0; i < func->params.size(); i++) {
            std::string paramType = func->params[i].second;
            if (paramType == "str") {
                code << "const char* " << func->params[i].first;
            } else {
                code << paramType << " " << func->params[i].first;
            }
            if (i < func->params.size() - 1) code << ", ";
        }
        code << ") {\n";
        
        declaredVars.clear();
        for (const auto& stmt : func->body) {
            emitStatement(stmt);
        }
        
        code << "    return 0;\n";
        code << "}\n\n";
    }

    void emitStatement(const StmtPtr& stmt) {
        if (auto print = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            // Check for stream expression: print("text" << x << "more")
            if (auto streamExpr = std::dynamic_pointer_cast<StreamExpr>(print->expr)) {
                code << "    printf(\"";
                // Build format string
                std::vector<std::string> args;
                for (const auto& part : streamExpr->parts) {
                    if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(part)) {
                        code << escapeString(strLit->value);
                    } else if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(part)) {
                        code << "%d";
                        args.push_back(std::to_string(intLit->value));
                    } else if (auto floatLit = std::dynamic_pointer_cast<FloatLiteral>(part)) {
                        code << "%f";
                        args.push_back(std::to_string(floatLit->value));
                    } else if (auto var = std::dynamic_pointer_cast<Variable>(part)) {
                        auto it = varTypes.find(var->name);
                        if (it != varTypes.end()) {
                            if (it->second == "int" || it->second == "bool") {
                                code << "%d";
                            } else if (it->second == "float") {
                                code << "%f";
                            } else if (it->second == "var") {
                                code << "%s";  // var prints as string
                            } else {
                                code << "%s";
                            }
                        } else {
                            code << "%s";
                        }
                        args.push_back(var->name);
                    } else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(part)) {
                        // Function calls - assume int return type by default
                        code << "%d";
                        // Capture the function call expression
                        std::string savedCode = code.str();
                        code.str("");
                        code.clear();
                        emitExpression(funcCall);
                        args.push_back(code.str());
                        code.str(savedCode);
                        code.clear();
                        code.seekp(0, std::ios::end);
                    } else if (auto binOp = std::dynamic_pointer_cast<BinaryOp>(part)) {
                        // Binary operations - assume int result
                        code << "%d";
                        std::string savedCode = code.str();
                        code.str("");
                        code.clear();
                        emitExpression(binOp);
                        args.push_back(code.str());
                        code.str(savedCode);
                        code.clear();
                        code.seekp(0, std::ios::end);
                    } else {
                        code << "%s";
                        // Capture expression using a separate stringstream
                        std::stringstream exprSS;
                        // Save current code state
                        std::string savedCode = code.str();
                        code.str("");
                        code.clear();
                        emitExprAsString(part);
                        args.push_back(code.str());
                        // Restore code state
                        code.str(savedCode);
                        code.clear();
                        code.seekp(0, std::ios::end);
                    }
                }
                code << "\"";
                for (const auto& arg : args) {
                    code << ", " << arg;
                }
                code << ");\n";
            } else if (auto var = std::dynamic_pointer_cast<Variable>(print->expr)) {
                // Type-aware printing
                auto it = varTypes.find(var->name);
                if (it != varTypes.end()) {
                    if (it->second == "int" || it->second == "bool") {
                        code << "    printf(\"%d\\n\", " << var->name << ");\n";
                    } else if (it->second == "float") {
                        code << "    printf(\"%f\\n\", " << var->name << ");\n";
                    } else if (it->second == "var") {
                        code << "    printf(\"%s\\n\", " << var->name << ".str);\n";
                    } else {
                        code << "    printf(\"%s\\n\", " << var->name << ");\n";
                    }
                } else {
                    code << "    printf(\"%s\\n\", " << var->name << ");\n";
                }
            } else if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(print->expr)) {
                code << "    printf(\"%ld\\n\", (long)" << intLit->value << ");\n";
            } else if (auto floatLit = std::dynamic_pointer_cast<FloatLiteral>(print->expr)) {
                code << "    printf(\"%f\\n\", " << floatLit->value << ");\n";
            } else {
                code << "    printf(\"%s\\n\", ";
                emitExprAsString(print->expr);
                code << ");\n";
            }
        } else if (auto inputStmt = std::dynamic_pointer_cast<InputStmt>(stmt)) {
            // Typed input statement
            if (!inputStmt->prompt.empty()) {
                code << "    printf(\"" << escapeString(inputStmt->prompt) << "\");\n";
            }
            if (!inputStmt->resultVar.empty()) {
                // Check if variable already has a known type
                std::string varType = inputStmt->varType;
                auto existingType = varTypes.find(inputStmt->resultVar);
                if (existingType != varTypes.end() && varType == "str") {
                    // Use existing type if input didn't specify one
                    varType = existingType->second;
                }
                varTypes[inputStmt->resultVar] = varType;  // Track variable type
                
                if (varType == "int") {
                    // Integer input
                    if (declaredVars.find(inputStmt->resultVar) == declaredVars.end()) {
                        code << "    int " << inputStmt->resultVar << ";\n";
                        declaredVars.insert(inputStmt->resultVar);
                    }
                    code << "    scanf(\"%d\", &" << inputStmt->resultVar << ");\n";
                    code << "    while(getchar() != '\\n');\n";  // Clear input buffer
                } else if (varType == "float") {
                    // Float input
                    if (declaredVars.find(inputStmt->resultVar) == declaredVars.end()) {
                        code << "    float " << inputStmt->resultVar << ";\n";
                        declaredVars.insert(inputStmt->resultVar);
                    }
                    code << "    scanf(\"%f\", &" << inputStmt->resultVar << ");\n";
                    code << "    while(getchar() != '\\n');\n";
                } else if (varType == "bool") {
                    // Bool input (accepts 0/1, true/false, yes/no)
                    if (declaredVars.find(inputStmt->resultVar) == declaredVars.end()) {
                        code << "    int " << inputStmt->resultVar << ";\n";
                        declaredVars.insert(inputStmt->resultVar);
                    }
                    code << "    { char __tmp[16]; fgets(__tmp, 16, stdin); __tmp[strcspn(__tmp, \"\\n\")] = 0;\n";
                    code << "      " << inputStmt->resultVar << " = (__tmp[0] == '1' || __tmp[0] == 't' || __tmp[0] == 'T' || __tmp[0] == 'y' || __tmp[0] == 'Y'); }\n";
                    varTypes[inputStmt->resultVar] = "bool";
                } else {
                    // String input (default)
                    if (declaredVars.find(inputStmt->resultVar) == declaredVars.end()) {
                        code << "    char " << inputStmt->resultVar << "[256];\n";
                        declaredVars.insert(inputStmt->resultVar);
                    }
                    code << "    fgets(" << inputStmt->resultVar << ", 256, stdin);\n";
                    code << "    " << inputStmt->resultVar << "[strcspn(" << inputStmt->resultVar << ", \"\\n\")] = 0;\n";
                }
            }
        } else if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(stmt)) {
            // Variable declaration: int x; float y = 3.14; str z; var c;
            if (declaredVars.find(varDecl->name) == declaredVars.end()) {
                if (varDecl->varType == "int") {
                    code << "    int " << varDecl->name;
                    if (varDecl->initialValue) {
                        code << " = ";
                        emitExpression(varDecl->initialValue);
                    } else {
                        code << " = 0";
                    }
                    code << ";\n";
                    varTypes[varDecl->name] = "int";
                } else if (varDecl->varType == "float") {
                    code << "    float " << varDecl->name;
                    if (varDecl->initialValue) {
                        code << " = ";
                        emitExpression(varDecl->initialValue);
                    } else {
                        code << " = 0.0f";
                    }
                    code << ";\n";
                    varTypes[varDecl->name] = "float";
                } else if (varDecl->varType == "bool") {
                    code << "    int " << varDecl->name;
                    if (varDecl->initialValue) {
                        code << " = ";
                        emitExpression(varDecl->initialValue);
                    } else {
                        code << " = 0";
                    }
                    code << ";\n";
                    varTypes[varDecl->name] = "bool";
                } else if (varDecl->varType == "str") {
                    // str type - use char* for function parameters, char[] for local vars
                    code << "    char " << varDecl->name << "[256]";
                    if (varDecl->initialValue) {
                        if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(varDecl->initialValue)) {
                            code << " = \"" << escapeString(strLit->value) << "\"";
                        }
                    } else {
                        code << " = \"\"";
                    }
                    code << ";\n";
                    varTypes[varDecl->name] = "str";
                } else if (varDecl->varType == "var") {
                    // Universal var type - stores string representation
                    code << "    struct { char str[256]; int ival; float fval; } " << varDecl->name;
                    if (varDecl->initialValue) {
                        if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(varDecl->initialValue)) {
                            code << " = {\"" << escapeString(strLit->value) << "\", 0, 0.0f}";
                        } else if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(varDecl->initialValue)) {
                            code << " = {\"\", " << intLit->value << ", 0.0f}";
                            code << ";\n    sprintf(" << varDecl->name << ".str, \"%ld\", (long)" << intLit->value << ")";
                        } else if (auto floatLit = std::dynamic_pointer_cast<FloatLiteral>(varDecl->initialValue)) {
                            code << " = {\"\", 0, " << floatLit->value << "f}";
                            code << ";\n    sprintf(" << varDecl->name << ".str, \"%f\", " << floatLit->value << ")";
                        } else {
                            code << " = {\"\", 0, 0.0f}";
                        }
                    } else {
                        code << " = {\"\", 0, 0.0f}";
                    }
                    code << ";\n";
                    varTypes[varDecl->name] = "var";
                } else {
                    // str
                    code << "    char " << varDecl->name << "[256]";
                    if (varDecl->initialValue) {
                        if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(varDecl->initialValue)) {
                            code << " = \"" << escapeString(strLit->value) << "\"";
                        }
                    } else {
                        code << " = \"\"";
                    }
                    code << ";\n";
                    varTypes[varDecl->name] = "str";
                }
                declaredVars.insert(varDecl->name);
            }
        } else if (auto typeConv = std::dynamic_pointer_cast<TypeConvert>(stmt)) {
            // Type conversion: float << x; int << y; str << z;
            if (typeConv->targetType == "int") {
                code << "    " << typeConv->varName << " = (int)" << typeConv->varName << ";\n";
                varTypes[typeConv->varName] = "int";
            } else if (typeConv->targetType == "float") {
                code << "    " << typeConv->varName << " = (float)" << typeConv->varName << ";\n";
                varTypes[typeConv->varName] = "float";
            } else if (typeConv->targetType == "str") {
                // Convert to string - need a temp buffer
                std::string tmpBuf = "__str_" + typeConv->varName;
                if (declaredVars.find(tmpBuf) == declaredVars.end()) {
                    code << "    char " << tmpBuf << "[64];\n";
                    declaredVars.insert(tmpBuf);
                }
                auto it = varTypes.find(typeConv->varName);
                if (it != varTypes.end() && (it->second == "int" || it->second == "bool")) {
                    code << "    sprintf(" << tmpBuf << ", \"%d\", " << typeConv->varName << ");\n";
                } else if (it != varTypes.end() && it->second == "float") {
                    code << "    sprintf(" << tmpBuf << ", \"%f\", " << typeConv->varName << ");\n";
                } else {
                    code << "    sprintf(" << tmpBuf << ", \"%s\", " << typeConv->varName << ");\n";
                }
                // Note: the string is now in tmpBuf, user should use that
            }
        } else if (auto fmtOut = std::dynamic_pointer_cast<FormatOutput>(stmt)) {
            // Format output: x >> 0x; x >> bin;
            if (fmtOut->format == "hex") {
                code << "    printf(\"0x%x\\n\", (unsigned int)" << fmtOut->varName << ");\n";
            } else if (fmtOut->format == "bin") {
                // Binary output - need helper
                code << "    { unsigned int __v = (unsigned int)" << fmtOut->varName << "; printf(\"0b\");\n";
                code << "      for(int __i = 31; __i >= 0; __i--) printf(\"%d\", (__v >> __i) & 1);\n";
                code << "      printf(\"\\n\"); }\n";
            } else if (fmtOut->format == "oct") {
                code << "    printf(\"0o%o\\n\", (unsigned int)" << fmtOut->varName << ");\n";
            } else {
                // dec (default)
                code << "    printf(\"%d\\n\", (int)" << fmtOut->varName << ");\n";
            }
        } else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            // Check for format conversion: decv = c >> 0x;
            if (auto fmtConv = std::dynamic_pointer_cast<FormatConvert>(assign->value)) {
                // Convert string to number in specified format
                auto srcType = varTypes.find(fmtConv->varName);
                std::string srcAccess = fmtConv->varName;
                if (srcType != varTypes.end() && srcType->second == "var") {
                    srcAccess = fmtConv->varName + ".str";
                }
                
                if (declaredVars.find(assign->name) == declaredVars.end()) {
                    code << "    long " << assign->name << ";\n";
                    declaredVars.insert(assign->name);
                    varTypes[assign->name] = "int";
                }
                
                // Parse string as number based on format
                if (fmtConv->format == "hex") {
                    code << "    " << assign->name << " = strtol(" << srcAccess << ", NULL, 16);\n";
                } else if (fmtConv->format == "bin") {
                    code << "    " << assign->name << " = strtol(" << srcAccess << ", NULL, 2);\n";
                } else if (fmtConv->format == "oct") {
                    code << "    " << assign->name << " = strtol(" << srcAccess << ", NULL, 8);\n";
                } else {
                    // dec - parse as decimal
                    code << "    " << assign->name << " = strtol(" << srcAccess << ", NULL, 10);\n";
                }
            } else if (auto arrLit = std::dynamic_pointer_cast<ArrayLiteral>(assign->value)) {
                // Array declaration: name[] = "string" becomes char name[] = {'a', 'b', 'c', 0};
                if (declaredVars.find(assign->name) == declaredVars.end()) {
                    code << "    char " << assign->name << "[] = {";
                    for (size_t i = 0; i < arrLit->elements.size(); i++) {
                        if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(arrLit->elements[i])) {
                            code << "'" << escapeString(strLit->value) << "'";
                        } else {
                            emitExpression(arrLit->elements[i]);
                        }
                        code << ", ";
                    }
                    code << "0};\n";
                    declaredVars.insert(assign->name);
                }
            } else {
                if (declaredVars.find(assign->name) == declaredVars.end()) {
                    code << "    int " << assign->name << " = ";
                    declaredVars.insert(assign->name);
                } else {
                    code << "    " << assign->name << " = ";
                }
                emitExpression(assign->value);
                code << ";\n";
            }
        } else if (auto arrAssign = std::dynamic_pointer_cast<ArrayAssignment>(stmt)) {
            code << "    " << arrAssign->name << "[";
            emitExpression(arrAssign->index);
            code << "] = ";
            // For char arrays, extract first char from string
            if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(arrAssign->value)) {
                if (!strLit->value.empty()) {
                    code << "'" << escapeString(strLit->value.substr(0, 1)) << "'";
                } else {
                    code << "0";
                }
            } else {
                emitExpression(arrAssign->value);
            }
            code << ";\n";
        } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            code << "    if (";
            emitExpression(ifStmt->condition);
            code << ") {\n";
            for (const auto& s : ifStmt->thenBlock) emitStatement(s);
            code << "    }";
            if (!ifStmt->elseBlock.empty()) {
                code << " else {\n";
                for (const auto& s : ifStmt->elseBlock) emitStatement(s);
                code << "    }";
            }
            code << "\n";
        } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            code << "    while (";
            emitExpression(whileStmt->condition);
            code << ") {\n";
            for (const auto& s : whileStmt->body) emitStatement(s);
            code << "    }\n";
        } else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            // Track loop variable as int
            varTypes[forStmt->var] = "int";
            declaredVars.insert(forStmt->var);
            code << "    for (int " << forStmt->var << " = ";
            emitExpression(forStmt->start);
            code << "; " << forStmt->var << " < ";
            emitExpression(forStmt->end);
            code << "; " << forStmt->var << "++) {\n";
            for (const auto& s : forStmt->body) emitStatement(s);
            code << "    }\n";
        } else if (auto retStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
            code << "    return";
            if (retStmt->value) {
                code << " ";
                emitExpression(retStmt->value);
            }
            code << ";\n";
        } else if (auto httpStmt = std::dynamic_pointer_cast<HttpStmt>(stmt)) {
            code << "    char* " << httpStmt->resultVar << " = __disp_http_get(";
            emitExpression(httpStmt->url);
            code << ");\n";
        } else if (auto keyStmt = std::dynamic_pointer_cast<KeypressStmt>(stmt)) {
            if (!keyStmt->resultVar.empty()) {
                code << "    int " << keyStmt->resultVar << " = __disp_get_keypress();\n";
            } else {
                code << "    __disp_get_keypress();\n";
            }
        } else if (auto hotkeyStmt = std::dynamic_pointer_cast<HotkeyStmt>(stmt)) {
            code << "    __disp_register_hotkey(";
            emitExpression(hotkeyStmt->key);
            code << ", " << hotkeyStmt->callback << ");\n";
        } else if (auto aiStmt = std::dynamic_pointer_cast<AIStmt>(stmt)) {
            code << "    char* " << aiStmt->resultVar << " = __disp_ai_query(";
            emitExpression(aiStmt->prompt);
            code << ");\n";
        } else if (auto gfxStmt = std::dynamic_pointer_cast<GraphicsStmt>(stmt)) {
            if (gfxStmt->command == "clear") {
                code << "    __disp_clear();\n";
            } else if (gfxStmt->command == "render") {
                code << "    __disp_render();\n";
            } else if (gfxStmt->command == "window_create") {
                code << "    __disp_window_create(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "gl_init") {
                code << "    __disp_gl_init();\n";
            } else if (gfxStmt->command == "set_color") {
                code << "    __disp_set_color(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "set_clear_color") {
                code << "    __disp_set_clear_color(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "draw_triangle") {
                code << "    __disp_draw_triangle(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "draw_quad") {
                code << "    __disp_draw_quad(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "fill_rect") {
                code << "    __disp_fill_rect(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "draw_circle") {
                code << "    __disp_draw_circle(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "fill_circle") {
                code << "    __disp_fill_circle(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "set_projection_2d") {
                code << "    __disp_set_projection_2d();\n";
            } else if (gfxStmt->command == "set_projection_3d") {
                code << "    __disp_set_projection_3d(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "translate") {
                code << "    __disp_translate(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "rotate") {
                code << "    __disp_rotate(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "scale") {
                code << "    __disp_scale(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "push_matrix") {
                code << "    __disp_push_matrix();\n";
            } else if (gfxStmt->command == "pop_matrix") {
                code << "    __disp_pop_matrix();\n";
            } else if (gfxStmt->command == "window_should_close") {
                code << "    __disp_window_should_close();\n";
            } else if (gfxStmt->command == "draw_pixel") {
                code << "    __disp_draw_pixel(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "draw_line") {
                code << "    __disp_draw_line(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "draw_rect") {
                code << "    __disp_draw_rect(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "draw_image") {
                // draw_image supports 3, 5, or 9 args like ctx.drawImage
                if (gfxStmt->args.size() == 3) {
                    // draw_image(id, x, y)
                    code << "    __disp_draw_image(";
                } else if (gfxStmt->args.size() == 5) {
                    // draw_image(id, x, y, w, h)
                    code << "    __disp_draw_image_scaled(";
                } else if (gfxStmt->args.size() >= 9) {
                    // draw_image(id, sx, sy, sw, sh, dx, dy, dw, dh)
                    code << "    __disp_draw_image_9(";
                } else {
                    code << "    __disp_draw_image(";
                }
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            } else if (gfxStmt->command == "play_sound") {
                code << "    __disp_play_sound(";
                emitExpression(gfxStmt->args[0]);
                if (gfxStmt->args.size() > 1) {
                    code << ", ";
                    emitExpression(gfxStmt->args[1]);
                } else {
                    code << ", 0";  // no loop by default
                }
                code << ");\n";
            } else if (gfxStmt->command == "stop_sound") {
                code << "    __disp_stop_sound(";
                for (size_t i = 0; i < gfxStmt->args.size(); i++) {
                    emitExpression(gfxStmt->args[i]);
                    if (i < gfxStmt->args.size() - 1) code << ", ";
                }
                code << ");\n";
            }
        } else if (auto iconStmt = std::dynamic_pointer_cast<IconStmt>(stmt)) {
            // Icon is handled at compile time, not runtime
            // Just emit a comment
            code << "    // App icon: " << iconStmt->iconPath << "\n";
        } else if (auto fromInc = std::dynamic_pointer_cast<FromIncludeStmt>(stmt)) {
            // From-include is handled at preprocessing, emit comment
            code << "    // from \"" << fromInc->path << "\" include ";
            for (size_t i = 0; i < fromInc->symbols.size(); i++) {
                code << fromInc->symbols[i];
                if (i < fromInc->symbols.size() - 1) code << ", ";
            }
            code << "\n";
        }
    }

    void emitExpression(const ExprPtr& expr) {
        if (auto intLit = std::dynamic_pointer_cast<IntLiteral>(expr)) {
            code << intLit->value;
        } else if (auto floatLit = std::dynamic_pointer_cast<FloatLiteral>(expr)) {
            code << floatLit->value;
        } else if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(expr)) {
            code << "\"" << escapeString(strLit->value) << "\"";
        } else if (auto boolLit = std::dynamic_pointer_cast<BoolLiteral>(expr)) {
            code << (boolLit->value ? "1" : "0");
        } else if (auto infLit = std::dynamic_pointer_cast<InfinityLiteral>(expr)) {
            code << "2147483647";  // Max 32-bit signed integer to represent infinity
        } else if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            code << var->name;
        } else if (auto arrAccess = std::dynamic_pointer_cast<ArrayAccess>(expr)) {
            code << arrAccess->name << "[";
            emitExpression(arrAccess->index);
            code << "]";
        } else if (auto binOp = std::dynamic_pointer_cast<BinaryOp>(expr)) {
            code << "(";
            emitExpression(binOp->left);
            code << " " << binOp->op << " ";
            emitExpression(binOp->right);
            code << ")";
        } else if (auto unaryOp = std::dynamic_pointer_cast<UnaryOp>(expr)) {
            code << unaryOp->op << "(";
            emitExpression(unaryOp->expr);
            code << ")";
        } else if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
            // Handle special input functions
            if (funcCall->name == "is_key_pressed" || funcCall->name == "is_key_just_pressed" || 
                funcCall->name == "is_key_released" || funcCall->name == "get_mouse_x" || 
                funcCall->name == "get_mouse_y" || funcCall->name == "is_mouse_pressed" || 
                funcCall->name == "is_mouse_just_pressed" || funcCall->name == "is_mouse_released" || 
                funcCall->name == "get_mouse_wheel") {
                needsKeypress = true;  // Ensure input system is included
            }
            
            // Map function names to C implementations
            std::string cFuncName = funcCall->name;
            if (funcCall->name == "is_key_pressed") cFuncName = "__disp_is_key_pressed";
            else if (funcCall->name == "is_key_just_pressed") cFuncName = "__disp_is_key_just_pressed";
            else if (funcCall->name == "is_key_released") cFuncName = "__disp_is_key_released";
            else if (funcCall->name == "get_mouse_x") cFuncName = "__disp_get_mouse_x";
            else if (funcCall->name == "get_mouse_y") cFuncName = "__disp_get_mouse_y";
            else if (funcCall->name == "is_mouse_pressed") cFuncName = "__disp_is_mouse_pressed";
            else if (funcCall->name == "is_mouse_just_pressed") cFuncName = "__disp_is_mouse_just_pressed";
            else if (funcCall->name == "is_mouse_released") cFuncName = "__disp_is_mouse_released";
            else if (funcCall->name == "get_mouse_wheel") cFuncName = "__disp_get_mouse_wheel";
            else if (funcCall->name == "set_mouse_position") cFuncName = "__disp_set_mouse_position";
            else if (funcCall->name == "show_cursor") cFuncName = "__disp_show_cursor";
            
            code << cFuncName << "(";
            for (size_t i = 0; i < funcCall->args.size(); i++) {
                emitExpression(funcCall->args[i]);
                if (i < funcCall->args.size() - 1) code << ", ";
            }
            code << ")";
        } else if (auto arrLit = std::dynamic_pointer_cast<ArrayLiteral>(expr)) {
            code << "{";
            for (size_t i = 0; i < arrLit->elements.size(); i++) {
                emitExpression(arrLit->elements[i]);
                if (i < arrLit->elements.size() - 1) code << ", ";
            }
            code << "}";
        } else if (auto loadImg = std::dynamic_pointer_cast<LoadImageExpr>(expr)) {
            code << "__disp_load_image(\"" << escapeString(loadImg->path) << "\")";
        } else if (auto sndExpr = std::dynamic_pointer_cast<SoundExpr>(expr)) {
            if (sndExpr->command == "load") {
                code << "__disp_load_sound(\"" << escapeString(sndExpr->path) << "\")";
            } else if (sndExpr->command == "play") {
                code << "__disp_play_sound(";
                for (size_t i = 0; i < sndExpr->args.size(); i++) {
                    emitExpression(sndExpr->args[i]);
                    if (i < sndExpr->args.size() - 1) code << ", ";
                }
                code << ")";
            } else if (sndExpr->command == "stop") {
                code << "__disp_stop_sound(";
                for (size_t i = 0; i < sndExpr->args.size(); i++) {
                    emitExpression(sndExpr->args[i]);
                    if (i < sndExpr->args.size() - 1) code << ", ";
                }
                code << ")";
            }
        }
    }

    void emitExprAsString(const ExprPtr& expr) {
        if (auto strLit = std::dynamic_pointer_cast<StringLiteral>(expr)) {
            code << "\"" << escapeString(strLit->value) << "\"";
        } else if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            // Just use the variable directly - works for both char[] and char*
            code << var->name;
        } else if (auto arrAccess = std::dynamic_pointer_cast<ArrayAccess>(expr)) {
            // Array element access - print as single char
            code << "({ char __buf[2] = {" << arrAccess->name << "[";
            emitExpression(arrAccess->index);
            code << "], 0}; __buf; })";
        } else {
            emitExpression(expr);
        }
    }

    std::string escapeString(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else if (c == '\t') result += "\\t";
            else result += c;
        }
        return result;
    }
};

} // namespace disp

#endif // DISPLEXITY_CODEGEN_H
