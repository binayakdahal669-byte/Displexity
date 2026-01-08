#ifndef DISPLEXITY_RENDERER3D_H
#define DISPLEXITY_RENDERER3D_H

#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <cstdint>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

namespace disp {

// 3D Vector
struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 cross(const Vec3& v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalize() const {
        float len = length();
        return len > 0 ? Vec3(x / len, y / len, z / len) : Vec3();
    }
};

// Triangle face
struct Triangle {
    Vec3 v[3];
    int color;
    Triangle() : color(7) { v[0] = v[1] = v[2] = Vec3(); }
    Triangle(Vec3 v0, Vec3 v1, Vec3 v2, int c = 7) : color(c) { v[0] = v0; v[1] = v1; v[2] = v2; }
};

// 4x4 Matrix for transformations
struct Mat4 {
    float m[4][4] = {{0}};
    static Mat4 identity() {
        Mat4 mat;
        mat.m[0][0] = mat.m[1][1] = mat.m[2][2] = mat.m[3][3] = 1.0f;
        return mat;
    }
    static Mat4 rotationX(float angle) {
        Mat4 mat = identity();
        mat.m[1][1] = std::cos(angle);
        mat.m[1][2] = std::sin(angle);
        mat.m[2][1] = -std::sin(angle);
        mat.m[2][2] = std::cos(angle);
        return mat;
    }
    static Mat4 rotationY(float angle) {
        Mat4 mat = identity();
        mat.m[0][0] = std::cos(angle);
        mat.m[0][2] = std::sin(angle);
        mat.m[2][0] = -std::sin(angle);
        mat.m[2][2] = std::cos(angle);
        return mat;
    }
    static Mat4 rotationZ(float angle) {
        Mat4 mat = identity();
        mat.m[0][0] = std::cos(angle);
        mat.m[0][1] = std::sin(angle);
        mat.m[1][0] = -std::sin(angle);
        mat.m[1][1] = std::cos(angle);
        return mat;
    }
    static Mat4 translation(float x, float y, float z) {
        Mat4 mat = identity();
        mat.m[3][0] = x; mat.m[3][1] = y; mat.m[3][2] = z;
        return mat;
    }
    static Mat4 projection(float fov, float aspect, float znear, float zfar) {
        Mat4 mat;
        float fovRad = 1.0f / std::tan(fov * 0.5f * 3.14159f / 180.0f);
        mat.m[0][0] = aspect * fovRad;
        mat.m[1][1] = fovRad;
        mat.m[2][2] = zfar / (zfar - znear);
        mat.m[3][2] = (-zfar * znear) / (zfar - znear);
        mat.m[2][3] = 1.0f;
        return mat;
    }
    Vec3 multiply(const Vec3& v) const {
        Vec3 out;
        out.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
        out.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
        out.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];
        float w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
        if (w != 0.0f) { out.x /= w; out.y /= w; out.z /= w; }
        return out;
    }
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 4; k++)
                    result.m[i][j] += m[i][k] * other.m[k][j];
        return result;
    }
};

// Mesh - collection of triangles
struct Mesh {
    std::vector<Triangle> tris;
    
    static Mesh cube() {
        Mesh mesh;
        // South
        mesh.tris.push_back(Triangle(Vec3(0,0,0), Vec3(0,1,0), Vec3(1,1,0), 1));
        mesh.tris.push_back(Triangle(Vec3(0,0,0), Vec3(1,1,0), Vec3(1,0,0), 1));
        // East
        mesh.tris.push_back(Triangle(Vec3(1,0,0), Vec3(1,1,0), Vec3(1,1,1), 2));
        mesh.tris.push_back(Triangle(Vec3(1,0,0), Vec3(1,1,1), Vec3(1,0,1), 2));
        // North
        mesh.tris.push_back(Triangle(Vec3(1,0,1), Vec3(1,1,1), Vec3(0,1,1), 3));
        mesh.tris.push_back(Triangle(Vec3(1,0,1), Vec3(0,1,1), Vec3(0,0,1), 3));
        // West
        mesh.tris.push_back(Triangle(Vec3(0,0,1), Vec3(0,1,1), Vec3(0,1,0), 4));
        mesh.tris.push_back(Triangle(Vec3(0,0,1), Vec3(0,1,0), Vec3(0,0,0), 4));
        // Top
        mesh.tris.push_back(Triangle(Vec3(0,1,0), Vec3(0,1,1), Vec3(1,1,1), 5));
        mesh.tris.push_back(Triangle(Vec3(0,1,0), Vec3(1,1,1), Vec3(1,1,0), 5));
        // Bottom
        mesh.tris.push_back(Triangle(Vec3(1,0,1), Vec3(0,0,1), Vec3(0,0,0), 6));
        mesh.tris.push_back(Triangle(Vec3(1,0,1), Vec3(0,0,0), Vec3(1,0,0), 6));
        return mesh;
    }
    
    static Mesh pyramid() {
        Mesh mesh;
        Vec3 apex(0.5f, 1.0f, 0.5f);
        mesh.tris.push_back(Triangle(Vec3(0,0,0), apex, Vec3(1,0,0), 1));
        mesh.tris.push_back(Triangle(Vec3(1,0,0), apex, Vec3(1,0,1), 2));
        mesh.tris.push_back(Triangle(Vec3(1,0,1), apex, Vec3(0,0,1), 3));
        mesh.tris.push_back(Triangle(Vec3(0,0,1), apex, Vec3(0,0,0), 4));
        mesh.tris.push_back(Triangle(Vec3(0,0,0), Vec3(1,0,0), Vec3(1,0,1), 5));
        mesh.tris.push_back(Triangle(Vec3(0,0,0), Vec3(1,0,1), Vec3(0,0,1), 5));
        return mesh;
    }
};

// Key codes for special keys
enum KeyCode {
    KEY_NONE = 0,
    KEY_UP = 256, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDN,
    KEY_INSERT, KEY_DELETE,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    KEY_CTRL_A = 1, KEY_CTRL_B = 2, KEY_CTRL_C = 3, KEY_CTRL_D = 4,
    KEY_CTRL_E = 5, KEY_CTRL_F = 6, KEY_CTRL_G = 7, KEY_CTRL_H = 8,
    KEY_CTRL_I = 9, KEY_CTRL_J = 10, KEY_CTRL_K = 11, KEY_CTRL_L = 12,
    KEY_CTRL_M = 13, KEY_CTRL_N = 14, KEY_CTRL_O = 15, KEY_CTRL_P = 16,
    KEY_CTRL_Q = 17, KEY_CTRL_R = 18, KEY_CTRL_S = 19, KEY_CTRL_T = 20,
    KEY_CTRL_U = 21, KEY_CTRL_V = 22, KEY_CTRL_W = 23, KEY_CTRL_X = 24,
    KEY_CTRL_Y = 25, KEY_CTRL_Z = 26,
    KEY_ESC = 27, KEY_ENTER = 13, KEY_TAB = 9, KEY_BACKSPACE = 8,
    KEY_SPACE = 32
};

// Camera for 3D navigation
struct Camera {
    Vec3 pos;
    float yaw, pitch;
    
    Camera() : pos(0, 0, -5), yaw(0), pitch(0) {}
    
    Vec3 forward() const {
        return Vec3(
            std::sin(yaw) * std::cos(pitch),
            std::sin(pitch),
            std::cos(yaw) * std::cos(pitch)
        ).normalize();
    }
    
    Vec3 right() const {
        return Vec3(std::cos(yaw), 0, -std::sin(yaw)).normalize();
    }
    
    void move(float fb, float lr, float ud) {
        Vec3 fwd = forward();
        Vec3 rgt = right();
        pos = pos + fwd * fb + rgt * lr + Vec3(0, ud, 0);
    }
    
    void rotate(float dyaw, float dpitch) {
        yaw += dyaw;
        pitch += dpitch;
        if (pitch > 1.5f) pitch = 1.5f;
        if (pitch < -1.5f) pitch = -1.5f;
    }
    
    Mat4 viewMatrix() const {
        Mat4 rot = Mat4::rotationY(-yaw) * Mat4::rotationX(-pitch);
        Mat4 trans = Mat4::translation(-pos.x, -pos.y, -pos.z);
        return trans * rot;
    }
};


// TUI Renderer - Text-based 3D rendering with double buffering
class TuiRenderer {
private:
    int width, height;
    std::vector<char> frontBuffer;
    std::vector<char> backBuffer;
    std::vector<float> depthBuffer;
    std::vector<int> colorBuffer;
    bool running;
    
#ifdef _WIN32
    HANDLE hConsole;
    HANDLE hConsoleIn;
    DWORD oldMode;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
#else
    struct termios oldTermios;
#endif

    const char* shadeChars = " .:-=+*#%@";
    
public:
    TuiRenderer(int w = 80, int h = 24) : width(w), height(h), running(true) {
        frontBuffer.resize(width * height, ' ');
        backBuffer.resize(width * height, ' ');
        depthBuffer.resize(width * height, 0.0f);
        colorBuffer.resize(width * height, 7);
    }
    
    ~TuiRenderer() { cleanup(); }
    
    void init() {
#ifdef _WIN32
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hConsoleIn, &oldMode);
        SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
        tcgetattr(STDIN_FILENO, &oldTermios);
        struct termios newTermios = oldTermios;
        newTermios.c_lflag &= ~(ICANON | ECHO);
        newTermios.c_cc[VMIN] = 0;
        newTermios.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
        std::cout << "\033[?25l";
#endif
        clear();
    }
    
    void cleanup() {
#ifdef _WIN32
        SetConsoleMode(hConsoleIn, oldMode);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        SetConsoleTextAttribute(hConsole, 7);
#else
        tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
        std::cout << "\033[?25h\033[0m";
#endif
    }
    
    int getKey() {
#ifdef _WIN32
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 224) {
                ch = _getch();
                switch (ch) {
                    case 72: return KEY_UP;
                    case 80: return KEY_DOWN;
                    case 75: return KEY_LEFT;
                    case 77: return KEY_RIGHT;
                    case 71: return KEY_HOME;
                    case 79: return KEY_END;
                    case 73: return KEY_PGUP;
                    case 81: return KEY_PGDN;
                    case 82: return KEY_INSERT;
                    case 83: return KEY_DELETE;
                    case 59: return KEY_F1;
                    case 60: return KEY_F2;
                    case 61: return KEY_F3;
                    case 62: return KEY_F4;
                    case 63: return KEY_F5;
                    case 64: return KEY_F6;
                    case 65: return KEY_F7;
                    case 66: return KEY_F8;
                    case 67: return KEY_F9;
                    case 68: return KEY_F10;
                    case 133: return KEY_F11;
                    case 134: return KEY_F12;
                }
                return ch + 256;
            }
            if (ch >= 1 && ch <= 26) return ch;
            return ch;
        }
        return KEY_NONE;
#else
        int ch = getchar();
        if (ch == EOF || ch == -1) return KEY_NONE;
        if (ch == 27) {
            int ch2 = getchar();
            if (ch2 == EOF || ch2 == -1) return KEY_ESC;
            if (ch2 == '[') {
                int ch3 = getchar();
                switch (ch3) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                    case 'H': return KEY_HOME;
                    case 'F': return KEY_END;
                    case '5': getchar(); return KEY_PGUP;
                    case '6': getchar(); return KEY_PGDN;
                }
            }
            return KEY_ESC;
        }
        if (ch >= 1 && ch <= 26) return ch;
        return ch;
#endif
    }
    
    void clear() {
        std::fill(backBuffer.begin(), backBuffer.end(), ' ');
        std::fill(depthBuffer.begin(), depthBuffer.end(), 0.0f);
        std::fill(colorBuffer.begin(), colorBuffer.end(), 7);
    }
    
    void setPixel(int x, int y, char c, float depth = 1.0f, int color = 7) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        int idx = y * width + x;
        if (depth > depthBuffer[idx]) {
            backBuffer[idx] = c;
            depthBuffer[idx] = depth;
            colorBuffer[idx] = color;
        }
    }
    
    void drawLine(int x0, int y0, int x1, int y1, char c, int color = 7) {
        int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        while (true) {
            setPixel(x0, y0, c, 1.0f, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
    
    void fillTriangle(int x0, int y0, float z0, int x1, int y1, float z1, 
                      int x2, int y2, float z2, char c, int color = 7) {
        if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); std::swap(z0, z1); }
        if (y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); std::swap(z0, z2); }
        if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); std::swap(z1, z2); }
        
        int totalHeight = y2 - y0;
        if (totalHeight == 0) return;
        
        for (int y = y0; y <= y2; y++) {
            bool secondHalf = y > y1 || y1 == y0;
            int segmentHeight = secondHalf ? y2 - y1 : y1 - y0;
            if (segmentHeight == 0) continue;
            
            float alpha = (float)(y - y0) / totalHeight;
            float beta = secondHalf ? (float)(y - y1) / segmentHeight : (float)(y - y0) / segmentHeight;
            
            int ax = x0 + (int)((x2 - x0) * alpha);
            int bx = secondHalf ? x1 + (int)((x2 - x1) * beta) : x0 + (int)((x1 - x0) * beta);
            float az = z0 + (z2 - z0) * alpha;
            float bz = secondHalf ? z1 + (z2 - z1) * beta : z0 + (z1 - z0) * beta;
            
            if (ax > bx) { std::swap(ax, bx); std::swap(az, bz); }
            
            for (int x = ax; x <= bx; x++) {
                float t = (bx == ax) ? 0 : (float)(x - ax) / (bx - ax);
                float z = az + (bz - az) * t;
                setPixel(x, y, c, z, color);
            }
        }
    }
    
    void present() {
#ifdef _WIN32
        COORD pos = {0, 0};
        SetConsoleCursorPosition(hConsole, pos);
        int lastColor = -1;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = y * width + x;
                if (colorBuffer[idx] != lastColor) {
                    SetConsoleTextAttribute(hConsole, colorBuffer[idx]);
                    lastColor = colorBuffer[idx];
                }
                putchar(backBuffer[idx]);
            }
            if (y < height - 1) putchar('\n');
        }
#else
        std::cout << "\033[H";
        int lastColor = -1;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = y * width + x;
                if (colorBuffer[idx] != lastColor) {
                    int fg = colorBuffer[idx] & 0x0F;
                    std::cout << "\033[" << (30 + (fg & 7)) << "m";
                    lastColor = colorBuffer[idx];
                }
                std::cout << backBuffer[idx];
            }
            if (y < height - 1) std::cout << '\n';
        }
#endif
        std::cout.flush();
        std::swap(frontBuffer, backBuffer);
    }
    
    void render(const Mesh& mesh, const Mat4& transform, const Mat4& projection) {
        Vec3 lightDir(0, 0, -1);
        lightDir = lightDir.normalize();
        
        for (const auto& tri : mesh.tris) {
            Vec3 v0 = transform.multiply(tri.v[0]);
            Vec3 v1 = transform.multiply(tri.v[1]);
            Vec3 v2 = transform.multiply(tri.v[2]);
            
            Vec3 line1 = v1 - v0;
            Vec3 line2 = v2 - v0;
            Vec3 normal = line1.cross(line2).normalize();
            
            Vec3 cameraRay = v0;
            if (normal.dot(cameraRay) >= 0) continue;
            
            float dp = std::max(0.1f, normal.dot(lightDir));
            int shadeIdx = (int)(dp * 9);
            if (shadeIdx > 9) shadeIdx = 9;
            char shadeChar = shadeChars[shadeIdx];
            
            Vec3 p0 = projection.multiply(v0);
            Vec3 p1 = projection.multiply(v1);
            Vec3 p2 = projection.multiply(v2);
            
            int sx0 = (int)((p0.x + 1) * 0.5f * width);
            int sy0 = (int)((p0.y + 1) * 0.5f * height);
            int sx1 = (int)((p1.x + 1) * 0.5f * width);
            int sy1 = (int)((p1.y + 1) * 0.5f * height);
            int sx2 = (int)((p2.x + 1) * 0.5f * width);
            int sy2 = (int)((p2.y + 1) * 0.5f * height);
            
            float avgZ = (p0.z + p1.z + p2.z) / 3.0f;
            fillTriangle(sx0, sy0, avgZ, sx1, sy1, avgZ, sx2, sy2, avgZ, shadeChar, tri.color);
        }
    }
    
    bool isRunning() const { return running; }
    void stop() { running = false; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void drawText(int x, int y, const std::string& text, int color = 7) {
        for (size_t i = 0; i < text.length(); i++) {
            setPixel(x + (int)i, y, text[i], 1.0f, color);
        }
    }
    
    void drawBox(int x, int y, int w, int h, int color = 7) {
        for (int i = x; i < x + w; i++) {
            setPixel(i, y, '-', 1.0f, color);
            setPixel(i, y + h - 1, '-', 1.0f, color);
        }
        for (int i = y; i < y + h; i++) {
            setPixel(x, i, '|', 1.0f, color);
            setPixel(x + w - 1, i, '|', 1.0f, color);
        }
        setPixel(x, y, '+', 1.0f, color);
        setPixel(x + w - 1, y, '+', 1.0f, color);
        setPixel(x, y + h - 1, '+', 1.0f, color);
        setPixel(x + w - 1, y + h - 1, '+', 1.0f, color);
    }
};


#ifdef _WIN32
// GUI Renderer - Win32 GDI based 3D rendering
class GuiRenderer {
private:
    int width, height;
    HWND hwnd;
    HDC hdc, hdcMem;
    HBITMAP hBitmap;
    HBITMAP hOldBitmap;
    std::vector<uint32_t> frameBuffer;
    std::vector<float> depthBuffer;
    bool running;
    MSG msg;
    int mouseX, mouseY;
    int lastMouseX, lastMouseY;
    bool mouseCapture;
    
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        GuiRenderer* renderer = (GuiRenderer*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        switch (msg) {
            case WM_DESTROY:
                if (renderer) renderer->running = false;
                PostQuitMessage(0);
                return 0;
            case WM_KEYDOWN:
                if (wParam == VK_ESCAPE) {
                    if (renderer) renderer->running = false;
                    DestroyWindow(hwnd);
                }
                return 0;
            case WM_MOUSEMOVE:
                if (renderer) {
                    renderer->mouseX = LOWORD(lParam);
                    renderer->mouseY = HIWORD(lParam);
                }
                return 0;
            case WM_LBUTTONDOWN:
                if (renderer && !renderer->mouseCapture) {
                    SetCapture(hwnd);
                    ShowCursor(FALSE);
                    renderer->mouseCapture = true;
                    renderer->lastMouseX = renderer->mouseX;
                    renderer->lastMouseY = renderer->mouseY;
                }
                return 0;
            case WM_RBUTTONDOWN:
                if (renderer && renderer->mouseCapture) {
                    ReleaseCapture();
                    ShowCursor(TRUE);
                    renderer->mouseCapture = false;
                }
                return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

public:
    GuiRenderer(int w = 800, int h = 600) : width(w), height(h), running(true), hwnd(NULL),
        mouseX(0), mouseY(0), lastMouseX(0), lastMouseY(0), mouseCapture(false) {
        frameBuffer.resize(width * height, 0);
        depthBuffer.resize(width * height, 0.0f);
    }
    
    ~GuiRenderer() { cleanup(); }
    
    bool init(const char* title = "Displexity 3D") {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "Disp3DClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClass(&wc);
        
        RECT rect = {0, 0, width, height};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        
        hwnd = CreateWindow("Disp3DClass", title, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, GetModuleHandle(NULL), NULL);
        
        if (!hwnd) return false;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
        
        hdc = GetDC(hwnd);
        hdcMem = CreateCompatibleDC(hdc);
        
        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        
        void* bits;
        hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
        hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        return true;
    }
    
    void cleanup() {
        if (mouseCapture) {
            ReleaseCapture();
            ShowCursor(TRUE);
        }
        if (hdcMem) {
            SelectObject(hdcMem, hOldBitmap);
            DeleteObject(hBitmap);
            DeleteDC(hdcMem);
            hdcMem = NULL;
        }
        if (hwnd && hdc) {
            ReleaseDC(hwnd, hdc);
            hdc = NULL;
        }
    }
    
    void clear(uint32_t color = 0x000000) {
        std::fill(frameBuffer.begin(), frameBuffer.end(), color);
        std::fill(depthBuffer.begin(), depthBuffer.end(), 0.0f);
    }
    
    void setPixel(int x, int y, uint32_t color, float depth = 1.0f) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        int idx = y * width + x;
        if (depth > depthBuffer[idx]) {
            frameBuffer[idx] = color;
            depthBuffer[idx] = depth;
        }
    }
    
    void drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
        int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        while (true) {
            setPixel(x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
    
    void fillTriangle(int x0, int y0, float z0, int x1, int y1, float z1,
                      int x2, int y2, float z2, uint32_t color) {
        if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); std::swap(z0, z1); }
        if (y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); std::swap(z0, z2); }
        if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); std::swap(z1, z2); }
        
        int totalHeight = y2 - y0;
        if (totalHeight == 0) return;
        
        for (int y = y0; y <= y2; y++) {
            bool secondHalf = y > y1 || y1 == y0;
            int segmentHeight = secondHalf ? y2 - y1 : y1 - y0;
            if (segmentHeight == 0) continue;
            
            float alpha = (float)(y - y0) / totalHeight;
            float beta = secondHalf ? (float)(y - y1) / segmentHeight : (float)(y - y0) / segmentHeight;
            
            int ax = x0 + (int)((x2 - x0) * alpha);
            int bx = secondHalf ? x1 + (int)((x2 - x1) * beta) : x0 + (int)((x1 - x0) * beta);
            float az = z0 + (z2 - z0) * alpha;
            float bz = secondHalf ? z1 + (z2 - z1) * beta : z0 + (z1 - z0) * beta;
            
            if (ax > bx) { std::swap(ax, bx); std::swap(az, bz); }
            
            for (int x = ax; x <= bx; x++) {
                float t = (bx == ax) ? 0 : (float)(x - ax) / (bx - ax);
                float z = az + (bz - az) * t;
                setPixel(x, y, color, z);
            }
        }
    }
    
    void render(const Mesh& mesh, const Mat4& transform, const Mat4& projection) {
        Vec3 lightDir(0, 0, -1);
        lightDir = lightDir.normalize();
        
        uint32_t colors[] = {0x404040, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF};
        
        for (const auto& tri : mesh.tris) {
            Vec3 v0 = transform.multiply(tri.v[0]);
            Vec3 v1 = transform.multiply(tri.v[1]);
            Vec3 v2 = transform.multiply(tri.v[2]);
            
            Vec3 line1 = v1 - v0;
            Vec3 line2 = v2 - v0;
            Vec3 normal = line1.cross(line2).normalize();
            
            Vec3 cameraRay = v0;
            if (normal.dot(cameraRay) >= 0) continue;
            
            float dp = std::max(0.1f, normal.dot(lightDir));
            
            Vec3 p0 = projection.multiply(v0);
            Vec3 p1 = projection.multiply(v1);
            Vec3 p2 = projection.multiply(v2);
            
            int sx0 = (int)((p0.x + 1) * 0.5f * width);
            int sy0 = (int)((p0.y + 1) * 0.5f * height);
            int sx1 = (int)((p1.x + 1) * 0.5f * width);
            int sy1 = (int)((p1.y + 1) * 0.5f * height);
            int sx2 = (int)((p2.x + 1) * 0.5f * width);
            int sy2 = (int)((p2.y + 1) * 0.5f * height);
            
            float avgZ = (p0.z + p1.z + p2.z) / 3.0f;
            
            uint32_t baseColor = colors[tri.color % 7];
            uint32_t r = (uint32_t)(((baseColor >> 16) & 0xFF) * dp);
            uint32_t g = (uint32_t)(((baseColor >> 8) & 0xFF) * dp);
            uint32_t b = (uint32_t)((baseColor & 0xFF) * dp);
            uint32_t shadedColor = (r << 16) | (g << 8) | b;
            
            fillTriangle(sx0, sy0, avgZ, sx1, sy1, avgZ, sx2, sy2, avgZ, shadedColor);
        }
    }
    
    void present() {
        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        
        SetDIBitsToDevice(hdc, 0, 0, width, height, 0, 0, 0, height,
                          frameBuffer.data(), &bmi, DIB_RGB_COLORS);
    }
    
    bool processEvents() {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return running;
    }
    
    int getKey() {
        if (GetAsyncKeyState(VK_UP) & 0x8000) return KEY_UP;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000) return KEY_DOWN;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000) return KEY_LEFT;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) return KEY_RIGHT;
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return KEY_ESC;
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) return KEY_SPACE;
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) return KEY_ENTER;
        for (int i = 0; i < 26; i++) {
            if ((GetAsyncKeyState('A' + i) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
                return KEY_CTRL_A + i;
        }
        for (int i = 0; i < 12; i++) {
            if (GetAsyncKeyState(VK_F1 + i) & 0x8000) return KEY_F1 + i;
        }
        // WASD keys
        if (GetAsyncKeyState('W') & 0x8000) return 'w';
        if (GetAsyncKeyState('A') & 0x8000) return 'a';
        if (GetAsyncKeyState('S') & 0x8000) return 's';
        if (GetAsyncKeyState('D') & 0x8000) return 'd';
        if (GetAsyncKeyState('Q') & 0x8000) return 'q';
        if (GetAsyncKeyState('E') & 0x8000) return 'e';
        return KEY_NONE;
    }
    
    void getMouseDelta(int& dx, int& dy) {
        if (mouseCapture) {
            dx = mouseX - lastMouseX;
            dy = mouseY - lastMouseY;
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            // Re-center mouse
            POINT center = {width / 2, height / 2};
            ClientToScreen(hwnd, &center);
            SetCursorPos(center.x, center.y);
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            lastMouseX = pt.x;
            lastMouseY = pt.y;
        } else {
            dx = dy = 0;
        }
    }
    
    bool isMouseCaptured() const { return mouseCapture; }
    bool isRunning() const { return running; }
    void stop() { running = false; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void drawText(int x, int y, const std::string& text, uint32_t color = 0xFFFFFF) {
        // Simple bitmap font rendering would go here
        // For now, use GDI text
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF));
        TextOutA(hdc, x, y, text.c_str(), (int)text.length());
    }
};
#endif


// C Code Emitter for 3D Renderers
class Renderer3DCodeEmitter {
public:
    static std::string emitTuiRenderer() {
        return R"(
/* Displexity TUI 3D Renderer - Generated C Code */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#define SLEEP(ms) Sleep(ms)
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define SLEEP(ms) usleep((ms) * 1000)
#endif

typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 v[3]; int color; } Triangle;
typedef struct { float m[4][4]; } Mat4;
typedef struct { Triangle* tris; int count; } Mesh;

typedef struct {
    int width, height;
    char* frontBuffer;
    char* backBuffer;
    float* depthBuffer;
    int* colorBuffer;
    int running;
#ifdef _WIN32
    HANDLE hConsole, hConsoleIn;
    DWORD oldMode;
#else
    struct termios oldTermios;
#endif
} TuiRenderer;

/* Vector math */
Vec3 vec3_add(Vec3 a, Vec3 b) { return (Vec3){a.x+b.x, a.y+b.y, a.z+b.z}; }
Vec3 vec3_sub(Vec3 a, Vec3 b) { return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
Vec3 vec3_mul(Vec3 v, float s) { return (Vec3){v.x*s, v.y*s, v.z*s}; }
float vec3_dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 vec3_cross(Vec3 a, Vec3 b) { return (Vec3){a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x}; }
float vec3_len(Vec3 v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
Vec3 vec3_norm(Vec3 v) { float l=vec3_len(v); return l>0?(Vec3){v.x/l,v.y/l,v.z/l}:(Vec3){0,0,0}; }

/* Matrix math */
Mat4 mat4_identity() { Mat4 m={{{0}}}; m.m[0][0]=m.m[1][1]=m.m[2][2]=m.m[3][3]=1.0f; return m; }
Mat4 mat4_rotY(float a) { Mat4 m=mat4_identity(); m.m[0][0]=cosf(a); m.m[0][2]=sinf(a); m.m[2][0]=-sinf(a); m.m[2][2]=cosf(a); return m; }
Mat4 mat4_rotX(float a) { Mat4 m=mat4_identity(); m.m[1][1]=cosf(a); m.m[1][2]=sinf(a); m.m[2][1]=-sinf(a); m.m[2][2]=cosf(a); return m; }
Mat4 mat4_trans(float x, float y, float z) { Mat4 m=mat4_identity(); m.m[3][0]=x; m.m[3][1]=y; m.m[3][2]=z; return m; }
Mat4 mat4_proj(float fov, float aspect, float znear, float zfar) {
    Mat4 m={{{0}}}; float f=1.0f/tanf(fov*0.5f*3.14159f/180.0f);
    m.m[0][0]=aspect*f; m.m[1][1]=f; m.m[2][2]=zfar/(zfar-znear); m.m[3][2]=(-zfar*znear)/(zfar-znear); m.m[2][3]=1.0f;
    return m;
}
Vec3 mat4_mulv(Mat4* m, Vec3 v) {
    Vec3 o; o.x=v.x*m->m[0][0]+v.y*m->m[1][0]+v.z*m->m[2][0]+m->m[3][0];
    o.y=v.x*m->m[0][1]+v.y*m->m[1][1]+v.z*m->m[2][1]+m->m[3][1];
    o.z=v.x*m->m[0][2]+v.y*m->m[1][2]+v.z*m->m[2][2]+m->m[3][2];
    float w=v.x*m->m[0][3]+v.y*m->m[1][3]+v.z*m->m[2][3]+m->m[3][3];
    if(w!=0.0f){o.x/=w;o.y/=w;o.z/=w;} return o;
}
Mat4 mat4_mul(Mat4* a, Mat4* b) {
    Mat4 r={{{0}}}; for(int i=0;i<4;i++)for(int j=0;j<4;j++)for(int k=0;k<4;k++)r.m[i][j]+=a->m[i][k]*b->m[k][j]; return r;
}

/* TUI Renderer */
TuiRenderer* tui_create(int w, int h) {
    TuiRenderer* r=(TuiRenderer*)malloc(sizeof(TuiRenderer)); r->width=w; r->height=h; r->running=1;
    int sz=w*h; r->frontBuffer=(char*)malloc(sz); r->backBuffer=(char*)malloc(sz);
    r->depthBuffer=(float*)malloc(sz*sizeof(float)); r->colorBuffer=(int*)malloc(sz*sizeof(int));
    memset(r->frontBuffer,' ',sz); memset(r->backBuffer,' ',sz); return r;
}
void tui_destroy(TuiRenderer* r) { free(r->frontBuffer); free(r->backBuffer); free(r->depthBuffer); free(r->colorBuffer); free(r); }
void tui_init(TuiRenderer* r) {
#ifdef _WIN32
    r->hConsole=GetStdHandle(STD_OUTPUT_HANDLE); r->hConsoleIn=GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(r->hConsoleIn,&r->oldMode); SetConsoleMode(r->hConsoleIn,ENABLE_EXTENDED_FLAGS);
    CONSOLE_CURSOR_INFO ci; GetConsoleCursorInfo(r->hConsole,&ci); ci.bVisible=FALSE; SetConsoleCursorInfo(r->hConsole,&ci);
#else
    tcgetattr(STDIN_FILENO,&r->oldTermios); struct termios nt=r->oldTermios;
    nt.c_lflag&=~(ICANON|ECHO); nt.c_cc[VMIN]=0; nt.c_cc[VTIME]=0;
    tcsetattr(STDIN_FILENO,TCSANOW,&nt); printf("\033[?25l");
#endif
}
void tui_cleanup(TuiRenderer* r) {
#ifdef _WIN32
    SetConsoleMode(r->hConsoleIn,r->oldMode); CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(r->hConsole,&ci); ci.bVisible=TRUE; SetConsoleCursorInfo(r->hConsole,&ci);
    SetConsoleTextAttribute(r->hConsole,7);
#else
    tcsetattr(STDIN_FILENO,TCSANOW,&r->oldTermios); printf("\033[?25h\033[0m");
#endif
}
void tui_clear(TuiRenderer* r) {
    int sz=r->width*r->height; memset(r->backBuffer,' ',sz);
    for(int i=0;i<sz;i++){r->depthBuffer[i]=0.0f;r->colorBuffer[i]=7;}
}
void tui_setpixel(TuiRenderer* r, int x, int y, char c, float d, int col) {
    if(x<0||x>=r->width||y<0||y>=r->height)return;
    int i=y*r->width+x; if(d>r->depthBuffer[i]){r->backBuffer[i]=c;r->depthBuffer[i]=d;r->colorBuffer[i]=col;}
}
void tui_present(TuiRenderer* r) {
#ifdef _WIN32
    COORD pos={0,0}; SetConsoleCursorPosition(r->hConsole,pos);
    for(int y=0;y<r->height;y++){for(int x=0;x<r->width;x++){int i=y*r->width+x;SetConsoleTextAttribute(r->hConsole,r->colorBuffer[i]);putchar(r->backBuffer[i]);}if(y<r->height-1)putchar('\n');}
#else
    printf("\033[H"); int lastCol=-1;
    for(int y=0;y<r->height;y++){for(int x=0;x<r->width;x++){int i=y*r->width+x;if(r->colorBuffer[i]!=lastCol){printf("\033[%d;%dm",30+(r->colorBuffer[i]&7),40);lastCol=r->colorBuffer[i];}putchar(r->backBuffer[i]);}if(y<r->height-1)putchar('\n');}
#endif
    fflush(stdout); char* tmp=r->frontBuffer; r->frontBuffer=r->backBuffer; r->backBuffer=tmp;
}
int tui_getkey(TuiRenderer* r) {
#ifdef _WIN32
    if(_kbhit()){int ch=_getch();if(ch==0||ch==224){ch=_getch();switch(ch){case 72:return 256;case 80:return 257;case 75:return 258;case 77:return 259;}return ch+256;}return ch;}return 0;
#else
    int ch=getchar(); if(ch==EOF||ch==-1)return 0;
    if(ch==27){int c2=getchar();if(c2=='['){int c3=getchar();switch(c3){case 'A':return 256;case 'B':return 257;case 'D':return 258;case 'C':return 259;}}return 27;}return ch;
#endif
}

/* Mesh */
Mesh mesh_cube() {
    Mesh m; m.count=12; m.tris=(Triangle*)malloc(12*sizeof(Triangle));
    Vec3 v[8]={{0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1}};
    int f[12][4]={{0,3,2,1},{1,2,6,2},{5,6,7,3},{4,7,3,4},{3,7,6,5},{4,0,1,6},{0,2,1,1},{1,6,5,2},{5,7,4,3},{4,3,0,4},{3,6,2,5},{4,1,5,6}};
    for(int i=0;i<12;i++){m.tris[i].v[0]=v[f[i][0]];m.tris[i].v[1]=v[f[i][1]];m.tris[i].v[2]=v[f[i][2]];m.tris[i].color=f[i][3];}
    return m;
}

/* Triangle fill */
void tui_filltri(TuiRenderer* r, int x0, int y0, float z0, int x1, int y1, float z1, int x2, int y2, float z2, char c, int col) {
    int tmp; if(y0>y1){tmp=x0;x0=x1;x1=tmp;tmp=y0;y0=y1;y1=tmp;float t=z0;z0=z1;z1=t;}
    if(y0>y2){tmp=x0;x0=x2;x2=tmp;tmp=y0;y0=y2;y2=tmp;float t=z0;z0=z2;z2=t;}
    if(y1>y2){tmp=x1;x1=x2;x2=tmp;tmp=y1;y1=y2;y2=tmp;float t=z1;z1=z2;z2=t;}
    int th=y2-y0; if(th==0)return;
    for(int y=y0;y<=y2;y++){int sh=(y>y1||y1==y0);int seg=sh?y2-y1:y1-y0;if(seg==0)continue;
        float a=(float)(y-y0)/th; float b=sh?(float)(y-y1)/seg:(float)(y-y0)/seg;
        int ax=x0+(int)((x2-x0)*a); int bx=sh?x1+(int)((x2-x1)*b):x0+(int)((x1-x0)*b);
        float az=z0+(z2-z0)*a; float bz=sh?z1+(z2-z1)*b:z0+(z1-z0)*b;
        if(ax>bx){tmp=ax;ax=bx;bx=tmp;float t=az;az=bz;bz=t;}
        for(int x=ax;x<=bx;x++){float t=(bx==ax)?0:(float)(x-ax)/(bx-ax);tui_setpixel(r,x,y,c,az+(bz-az)*t,col);}
    }
}

/* Render mesh */
void tui_render(TuiRenderer* r, Mesh* mesh, Mat4* transform, Mat4* proj) {
    const char* shade=" .:-=+*#%@"; Vec3 light={0,0,-1}; light=vec3_norm(light);
    for(int i=0;i<mesh->count;i++){Triangle* tri=&mesh->tris[i];
        Vec3 v0=mat4_mulv(transform,tri->v[0]); Vec3 v1=mat4_mulv(transform,tri->v[1]); Vec3 v2=mat4_mulv(transform,tri->v[2]);
        Vec3 n=vec3_norm(vec3_cross(vec3_sub(v1,v0),vec3_sub(v2,v0)));
        if(vec3_dot(n,v0)>=0)continue; float dp=vec3_dot(n,light); if(dp<0.1f)dp=0.1f;
        int si=(int)(dp*9); if(si>9)si=9;
        Vec3 p0=mat4_mulv(proj,v0),p1=mat4_mulv(proj,v1),p2=mat4_mulv(proj,v2);
        int sx0=(int)((p0.x+1)*0.5f*r->width),sy0=(int)((p0.y+1)*0.5f*r->height);
        int sx1=(int)((p1.x+1)*0.5f*r->width),sy1=(int)((p1.y+1)*0.5f*r->height);
        int sx2=(int)((p2.x+1)*0.5f*r->width),sy2=(int)((p2.y+1)*0.5f*r->height);
        float avgZ=(p0.z+p1.z+p2.z)/3.0f;
        tui_filltri(r,sx0,sy0,avgZ,sx1,sy1,avgZ,sx2,sy2,avgZ,shade[si],tri->color);
    }
}
)";
    }
    
    static std::string emitGuiRenderer() {
        return R"(
/* Displexity GUI 3D Renderer - Generated C Code (Windows) */
#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 v[3]; int color; } Triangle;
typedef struct { float m[4][4]; } Mat4;
typedef struct { Triangle* tris; int count; } Mesh;

typedef struct {
    int width, height;
    HWND hwnd; HDC hdc;
    unsigned int* frameBuffer;
    float* depthBuffer;
    int running;
} GuiRenderer;

static GuiRenderer* g_renderer = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg){case WM_DESTROY:if(g_renderer)g_renderer->running=0;PostQuitMessage(0);return 0;
    case WM_KEYDOWN:if(wParam==VK_ESCAPE){if(g_renderer)g_renderer->running=0;DestroyWindow(hwnd);}return 0;}
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

GuiRenderer* gui_create(int w, int h, const char* title) {
    GuiRenderer* r=(GuiRenderer*)malloc(sizeof(GuiRenderer)); r->width=w; r->height=h; r->running=1;
    r->frameBuffer=(unsigned int*)malloc(w*h*sizeof(unsigned int));
    r->depthBuffer=(float*)malloc(w*h*sizeof(float));
    WNDCLASS wc={0}; wc.lpfnWndProc=WndProc; wc.hInstance=GetModuleHandle(NULL);
    wc.lpszClassName="Disp3D"; wc.hCursor=LoadCursor(NULL,IDC_ARROW); RegisterClass(&wc);
    RECT rect={0,0,w,h}; AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
    r->hwnd=CreateWindow("Disp3D",title,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
        rect.right-rect.left,rect.bottom-rect.top,NULL,NULL,GetModuleHandle(NULL),NULL);
    r->hdc=GetDC(r->hwnd); g_renderer=r; ShowWindow(r->hwnd,SW_SHOW); return r;
}
void gui_destroy(GuiRenderer* r) { ReleaseDC(r->hwnd,r->hdc); free(r->frameBuffer); free(r->depthBuffer); free(r); }
void gui_clear(GuiRenderer* r, unsigned int color) { int sz=r->width*r->height; for(int i=0;i<sz;i++){r->frameBuffer[i]=color;r->depthBuffer[i]=0.0f;} }
void gui_setpixel(GuiRenderer* r, int x, int y, unsigned int color, float d) {
    if(x<0||x>=r->width||y<0||y>=r->height)return; int i=y*r->width+x;
    if(d>r->depthBuffer[i]){r->frameBuffer[i]=color;r->depthBuffer[i]=d;}
}
void gui_present(GuiRenderer* r) {
    BITMAPINFO bmi={0}; bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth=r->width; bmi.bmiHeader.biHeight=-r->height;
    bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32;
    SetDIBitsToDevice(r->hdc,0,0,r->width,r->height,0,0,0,r->height,r->frameBuffer,&bmi,DIB_RGB_COLORS);
}
int gui_process(GuiRenderer* r) { MSG msg; while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);} return r->running; }
int gui_getkey() {
    if(GetAsyncKeyState(VK_UP)&0x8000)return 256; if(GetAsyncKeyState(VK_DOWN)&0x8000)return 257;
    if(GetAsyncKeyState(VK_LEFT)&0x8000)return 258; if(GetAsyncKeyState(VK_RIGHT)&0x8000)return 259;
    if(GetAsyncKeyState(VK_ESCAPE)&0x8000)return 27; if(GetAsyncKeyState(VK_SPACE)&0x8000)return 32;
    if(GetAsyncKeyState('W')&0x8000)return 'w'; if(GetAsyncKeyState('A')&0x8000)return 'a';
    if(GetAsyncKeyState('S')&0x8000)return 's'; if(GetAsyncKeyState('D')&0x8000)return 'd';
    return 0;
}
void gui_filltri(GuiRenderer* r, int x0, int y0, float z0, int x1, int y1, float z1, int x2, int y2, float z2, unsigned int color) {
    int tmp; if(y0>y1){tmp=x0;x0=x1;x1=tmp;tmp=y0;y0=y1;y1=tmp;float t=z0;z0=z1;z1=t;}
    if(y0>y2){tmp=x0;x0=x2;x2=tmp;tmp=y0;y0=y2;y2=tmp;float t=z0;z0=z2;z2=t;}
    if(y1>y2){tmp=x1;x1=x2;x2=tmp;tmp=y1;y1=y2;y2=tmp;float t=z1;z1=z2;z2=t;}
    int th=y2-y0; if(th==0)return;
    for(int y=y0;y<=y2;y++){int sh=(y>y1||y1==y0);int seg=sh?y2-y1:y1-y0;if(seg==0)continue;
        float a=(float)(y-y0)/th; float b=sh?(float)(y-y1)/seg:(float)(y-y0)/seg;
        int ax=x0+(int)((x2-x0)*a); int bx=sh?x1+(int)((x2-x1)*b):x0+(int)((x1-x0)*b);
        float az=z0+(z2-z0)*a; float bz=sh?z1+(z2-z1)*b:z0+(z1-z0)*b;
        if(ax>bx){tmp=ax;ax=bx;bx=tmp;float t=az;az=bz;bz=t;}
        for(int x=ax;x<=bx;x++){float t=(bx==ax)?0:(float)(x-ax)/(bx-ax);gui_setpixel(r,x,y,color,az+(bz-az)*t);}
    }
}
#endif
)";
    }
    
    static std::string emitDemo() {
        return R"(
/* Displexity 3D Demo - Rotating Cube with Camera */
int main() {
    TuiRenderer* r = tui_create(80, 24);
    tui_init(r);
    
    Mesh cube = mesh_cube();
    Mat4 proj = mat4_proj(90.0f, (float)r->height / r->width * 2.0f, 0.1f, 1000.0f);
    float angle = 0.0f;
    float camX = 0, camY = 0, camZ = -3;
    
    while (r->running) {
        int key = tui_getkey(r);
        if (key == 27 || key == 'q') r->running = 0;
        if (key == 'w') camZ += 0.1f;
        if (key == 's') camZ -= 0.1f;
        if (key == 'a') camX -= 0.1f;
        if (key == 'd') camX += 0.1f;
        if (key == 'e') camY += 0.1f;
        if (key == 'q' && key != 27) camY -= 0.1f;
        if (key == 256) angle -= 0.1f;  /* UP */
        if (key == 257) angle += 0.1f;  /* DOWN */
        
        tui_clear(r);
        
        Mat4 rotX = mat4_rotX(angle * 0.5f);
        Mat4 rotY = mat4_rotY(angle);
        Mat4 trans = mat4_trans(-0.5f + camX, -0.5f + camY, 3.0f + camZ);
        Mat4 world = mat4_mul(&rotX, &rotY);
        world = mat4_mul(&world, &trans);
        
        tui_render(r, &cube, &world, &proj);
        tui_present(r);
        
        angle += 0.03f;
        SLEEP(16);
    }
    
    tui_cleanup(r);
    tui_destroy(r);
    free(cube.tris);
    return 0;
}
)";
    }
};

} // namespace disp

#endif // DISPLEXITY_RENDERER3D_H
