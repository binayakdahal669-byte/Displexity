#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

// Graphics runtime with OpenGL support
#ifdef _WIN32
#include <gl/gl.h>
#include <math.h>
#pragma comment(lib, "opengl32.lib")
HWND __disp_hwnd = NULL;
HDC __disp_hdc = NULL;
HGLRC __disp_hglrc = NULL;
int __disp_width = 800, __disp_height = 600;
int __disp_gl_enabled = 0;

LRESULT CALLBACK __disp_wndproc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch(m) {
        case WM_DESTROY: PostQuitMessage(0); return 0;
        case WM_SIZE: __disp_width = LOWORD(l); __disp_height = HIWORD(l);
                      if (__disp_gl_enabled) glViewport(0, 0, __disp_width, __disp_height); break;
        case WM_KEYDOWN: /* Handle key events */ break;
    }
    return DefWindowProcA(h, m, w, l);
}

void __disp_graphics_init() {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = __disp_wndproc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "DISP";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);
}

void __disp_window_create(const char* title, int w, int h) {
    __disp_width = w; __disp_height = h;
    __disp_hwnd = CreateWindowA("DISP", title, WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, GetModuleHandle(NULL), NULL);
    __disp_hdc = GetDC(__disp_hwnd);
}

void __disp_gl_init() {
    if (!__disp_hdc) return;
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    int format = ChoosePixelFormat(__disp_hdc, &pfd);
    SetPixelFormat(__disp_hdc, format, &pfd);
    __disp_hglrc = wglCreateContext(__disp_hdc);
    wglMakeCurrent(__disp_hdc, __disp_hglrc);
    __disp_gl_enabled = 1;
    glViewport(0, 0, __disp_width, __disp_height);
    glEnable(GL_DEPTH_TEST);
}

void __disp_clear() {
    if (__disp_gl_enabled) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else if (__disp_hdc) {
        RECT r = {0, 0, __disp_width, __disp_height};
        FillRect(__disp_hdc, &r, (HBRUSH)(COLOR_WINDOW+1));
    }
}

void __disp_set_clear_color(float r, float g, float b, float a) {
    if (__disp_gl_enabled) glClearColor(r, g, b, a);
}

void __disp_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    if (__disp_gl_enabled) {
        glBegin(GL_TRIANGLES);
        glVertex2f(x1, y1); glVertex2f(x2, y2); glVertex2f(x3, y3);
        glEnd();
    }
}

void __disp_draw_quad(float x, float y, float w, float h) {
    if (__disp_gl_enabled) {
        glBegin(GL_QUADS);
        glVertex2f(x, y); glVertex2f(x+w, y);
        glVertex2f(x+w, y+h); glVertex2f(x, y+h);
        glEnd();
    }
}

void __disp_set_color(float r, float g, float b) {
    if (__disp_gl_enabled) glColor3f(r, g, b);
}

void __disp_draw_pixel(int x, int y) {
    if (__disp_gl_enabled) {
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();
    } else if (__disp_hdc) {
        SetPixel(__disp_hdc, x, y, RGB(0,0,0));
    }
}

void __disp_draw_line(int x1, int y1, int x2, int y2) {
    if (__disp_gl_enabled) {
        glBegin(GL_LINES);
        glVertex2i(x1, y1); glVertex2i(x2, y2);
        glEnd();
    } else if (__disp_hdc) {
        MoveToEx(__disp_hdc, x1, y1, NULL); LineTo(__disp_hdc, x2, y2);
    }
}

void __disp_draw_rect(int x, int y, int w, int h) {
    if (__disp_gl_enabled) {
        glBegin(GL_LINE_LOOP);
        glVertex2i(x, y); glVertex2i(x+w, y);
        glVertex2i(x+w, y+h); glVertex2i(x, y+h);
        glEnd();
    } else if (__disp_hdc) {
        Rectangle(__disp_hdc, x, y, x+w, y+h);
    }
}

void __disp_fill_rect(int x, int y, int w, int h) {
    if (__disp_gl_enabled) {
        glBegin(GL_QUADS);
        glVertex2i(x, y); glVertex2i(x+w, y);
        glVertex2i(x+w, y+h); glVertex2i(x, y+h);
        glEnd();
    }
}

void __disp_draw_circle(int cx, int cy, int radius) {
    if (__disp_gl_enabled) {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
        }
        glEnd();
    }
}

void __disp_fill_circle(int cx, int cy, int radius) {
    if (__disp_gl_enabled) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx, cy);
        for (int i = 0; i <= 360; i += 5) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
        }
        glEnd();
    }
}

void __disp_set_projection_2d() {
    if (__disp_gl_enabled) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, __disp_width, __disp_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}

void __disp_set_projection_3d(float fov, float near_plane, float far_plane) {
    if (__disp_gl_enabled) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = (float)__disp_width / (float)__disp_height;
        float f = 1.0f / tan(fov * 3.14159f / 360.0f);
        glFrustum(-aspect/f, aspect/f, -1.0f/f, 1.0f/f, near_plane, far_plane);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}

void __disp_translate(float x, float y, float z) {
    if (__disp_gl_enabled) glTranslatef(x, y, z);
}

void __disp_rotate(float angle, float x, float y, float z) {
    if (__disp_gl_enabled) glRotatef(angle, x, y, z);
}

void __disp_scale(float x, float y, float z) {
    if (__disp_gl_enabled) glScalef(x, y, z);
}

void __disp_push_matrix() {
    if (__disp_gl_enabled) glPushMatrix();
}

void __disp_pop_matrix() {
    if (__disp_gl_enabled) glPopMatrix();
}

int __disp_window_should_close() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return 1;
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            PostQuitMessage(0); return 1;
        }
        TranslateMessage(&msg); DispatchMessage(&msg);
    }
    return 0;
}

void __disp_render() {
    if (__disp_gl_enabled) {
        SwapBuffers(__disp_hdc);
    }
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        TranslateMessage(&msg); DispatchMessage(&msg);
    }
    Sleep(16); // ~60 FPS
}

void __disp_graphics_cleanup() {
    if (__disp_hglrc) { wglMakeCurrent(NULL, NULL); wglDeleteContext(__disp_hglrc); }
    if (__disp_hdc) ReleaseDC(__disp_hwnd, __disp_hdc);
    if (__disp_hwnd) DestroyWindow(__disp_hwnd);
}
#else
// Linux OpenGL support (stub)
void __disp_graphics_init() { printf("Graphics init\n"); }
void __disp_window_create(const char* t, int w, int h) { printf("Window: %s %dx%d\n", t, w, h); }
void __disp_gl_init() { printf("OpenGL init\n"); }
void __disp_clear() { printf("Clear\n"); }
void __disp_set_clear_color(float r, float g, float b, float a) {}
void __disp_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {}
void __disp_draw_quad(float x, float y, float w, float h) {}
void __disp_set_color(float r, float g, float b) {}
void __disp_draw_pixel(int x, int y) {}
void __disp_draw_line(int x1, int y1, int x2, int y2) {}
void __disp_draw_rect(int x, int y, int w, int h) {}
void __disp_fill_rect(int x, int y, int w, int h) {}
void __disp_draw_circle(int cx, int cy, int radius) {}
void __disp_fill_circle(int cx, int cy, int radius) {}
void __disp_set_projection_2d() {}
void __disp_set_projection_3d(float fov, float near_plane, float far_plane) {}
void __disp_translate(float x, float y, float z) {}
void __disp_rotate(float angle, float x, float y, float z) {}
void __disp_scale(float x, float y, float z) {}
void __disp_push_matrix() {}
void __disp_pop_matrix() {}
int __disp_window_should_close() { return 0; }
void __disp_render() {}
void __disp_graphics_cleanup() {}
#endif



int main(int argc, char** argv) {
    __disp_graphics_init();
    __disp_window_create("Input Test", 800, 600);
    __disp_gl_init();
    __disp_set_projection_2d();
    printf("%s\n", "Input Test Started - Press ESC to exit");
    printf("%s\n", "Use WASD keys, mouse buttons, and arrow keys");
    int running = 1;
    while ((running == 1)) {
    __disp_clear();
    __disp_set_clear_color(0.1, 0.1, 0.2, 1);
    if (is_key_pressed(87)) {
    __disp_set_color(1, 0, 0);
    __disp_fill_rect(100, 100, 50, 50);
    printf("%s\n", "W key pressed!");
    }
    if (is_key_pressed(65)) {
    __disp_set_color(0, 1, 0);
    __disp_fill_rect(50, 150, 50, 50);
    printf("%s\n", "A key pressed!");
    }
    if (is_key_pressed(83)) {
    __disp_set_color(0, 0, 1);
    __disp_fill_rect(100, 200, 50, 50);
    printf("%s\n", "S key pressed!");
    }
    if (is_key_pressed(68)) {
    __disp_set_color(1, 1, 0);
    __disp_fill_rect(150, 150, 50, 50);
    printf("%s\n", "D key pressed!");
    }
    int mouse_x = get_mouse_x();
    int mouse_y = get_mouse_y();
    __disp_set_color(1, 1, 1);
    __disp_fill_circle(mouse_x, mouse_y, 10);
    if (is_mouse_pressed(0)) {
    __disp_set_color(1, 0, 1);
    __disp_fill_circle(mouse_x, mouse_y, 20);
    printf("%s\n", ((("Left mouse button pressed at: " + mouse_x) + ", ") + mouse_y));
    }
    if (is_mouse_pressed(1)) {
    __disp_set_color(0, 1, 1);
    __disp_fill_rect((mouse_x - 15), (mouse_y - 15), 30, 30);
    printf("%s\n", "Right mouse button pressed!");
    }
    if (is_key_pressed(37)) {
    __disp_set_color(1, 0.5, 0);
    __disp_draw_triangle(300, 300, 280, 320, 280, 280);
    printf("%s\n", "Left arrow pressed!");
    }
    if (is_key_pressed(39)) {
    __disp_set_color(1, 0.5, 0);
    __disp_draw_triangle(400, 300, 420, 280, 420, 320);
    printf("%s\n", "Right arrow pressed!");
    }
    if (is_key_pressed(38)) {
    __disp_set_color(1, 0.5, 0);
    __disp_draw_triangle(350, 250, 330, 270, 370, 270);
    printf("%s\n", "Up arrow pressed!");
    }
    if (is_key_pressed(40)) {
    __disp_set_color(1, 0.5, 0);
    __disp_draw_triangle(350, 350, 330, 330, 370, 330);
    printf("%s\n", "Down arrow pressed!");
    }
    if (is_key_pressed(27)) {
    running = 0;
    }
    __disp_render();
    }
    printf("%s\n", "Input test completed!");
    __disp_graphics_cleanup();
    return 0;
}
