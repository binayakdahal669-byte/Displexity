#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

// Graphics runtime with OpenGL support
#ifdef _WIN32
#include <gl/gl.h>
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

void __disp_set_projection_3d(float fov, float near, float far) {
    if (__disp_gl_enabled) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = (float)__disp_width / (float)__disp_height;
        float f = 1.0f / tan(fov * 3.14159f / 360.0f);
        glFrustum(-aspect/f, aspect/f, -1.0f/f, 1.0f/f, near, far);
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
        TranslateMessage(&msg); DispatchMessage(&msg);
    }
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
void __disp_set_projection_3d(float fov, float near, float far) {}
void __disp_translate(float x, float y, float z) {}
void __disp_rotate(float angle, float x, float y, float z) {}
void __disp_scale(float x, float y, float z) {}
void __disp_push_matrix() {}
void __disp_pop_matrix() {}
int __disp_window_should_close() { return 0; }
void __disp_render() {}
void __disp_graphics_cleanup() {}
#endif

void __disp_clear() {}
void __disp_draw_pixel(int x, int y) {}
void __disp_draw_line(int x1, int y1, int x2, int y2) {}
void __disp_draw_rect(int x, int y, int w, int h) {}
void __disp_set_color(int r, int g, int b) {}
void __disp_render() {}
void __disp_graphics_cleanup() {}
#endif

int init_engine(str title, int width, int height);
int start_frame();
int end_frame();
int draw_background();
int init_player();
int update_player();
int render_player();
int init_enemies();
int update_enemies();
int render_enemies();
int init_engine(str title, int width, int height);
int start_frame();
int end_frame();
int draw_background();
int init_player();
int update_player();
int render_player();
int init_enemies();
int update_enemies();
int render_enemies();

int init_engine(str title, int width, int height) {
    __disp_window_create(title, width, height);
    __disp_gl_init();
    __disp_set_projection_2d();
    __disp_set_clear_color(0.1, 0.2, 0.3, 1);
    printf("Engine initialized: %s\n", title);
    return 0;
}

int start_frame() {
    __disp_clear();
    return 0;
}

int end_frame() {
    __disp_render();
    return 0;
}

int draw_background() {
    __disp_set_color(0.2, 0.2, 0.4);
    __disp_fill_rect(0, 0, 800, 600);
    __disp_set_color(1, 1, 1);
    for (int i = 0; i < 50; i++) {
    int star_x = ((i * 37) % 800);
    int star_y = ((i * 73) % 600);
    __disp_fill_circle(star_x, star_y, 1);
    }
    return 0;
}

int init_player() {
    int player_x = 400;
    int player_y = 300;
    int player_vx = 2;
    int player_vy = 1.5;
    printf("%s\n", "Player initialized\n");
    return 0;
}

int update_player() {
    int player_x = (player_x + player_vx);
    int player_y = (player_y + player_vy);
    if (((player_x < 20) || (player_x > 780))) {
    int player_vx = (0 - player_vx);
    }
    if (((player_y < 20) || (player_y > 580))) {
    int player_vy = (0 - player_vy);
    }
    return 0;
}

int render_player() {
    __disp_set_color(0, 0.5, 1);
    __disp_fill_circle(player_x, player_y, 15);
    __disp_set_color(1, 1, 1);
    __disp_draw_circle(player_x, player_y, 15);
    return 0;
}

int init_enemies() {
    int enemy1_x = 100;
    int enemy1_y = 100;
    int enemy2_x = 600;
    int enemy2_y = 400;
    int enemy3_x = 300;
    int enemy3_y = 200;
    printf("%s\n", "Enemies initialized\n");
    return 0;
}

int update_enemies() {
    int enemy1_x = (enemy1_x + 3);
    if ((enemy1_x > 800)) {
    enemy1_x = -(30);
    }
    int enemy2_y = (enemy2_y - 2);
    if ((enemy2_y < -(30))) {
    enemy2_y = 630;
    }
    int enemy3_x = (400 + (150 * cos((enemy1_x * 0.02))));
    int enemy3_y = (300 + (100 * sin((enemy1_x * 0.02))));
    return 0;
}

int render_enemies() {
    __disp_set_color(1, 0, 0);
    __disp_fill_rect(enemy1_x, enemy1_y, 25, 25);
    __disp_set_color(1, 0.5, 0);
    __disp_draw_triangle(enemy2_x, enemy2_y, (enemy2_x - 15), (enemy2_y + 20), (enemy2_x + 15), (enemy2_y + 20));
    __disp_set_color(0.8, 0, 0.8);
    __disp_draw_triangle(enemy3_x, (enemy3_y - 12), (enemy3_x - 12), enemy3_y, (enemy3_x + 12), enemy3_y);
    __disp_draw_triangle(enemy3_x, (enemy3_y + 12), (enemy3_x - 12), enemy3_y, (enemy3_x + 12), enemy3_y);
    return 0;
}

int init_engine(str title, int width, int height) {
    __disp_window_create(title, width, height);
    __disp_gl_init();
    __disp_set_projection_2d();
    __disp_set_clear_color(0.1, 0.2, 0.3, 1);
    printf("Engine initialized: %s\n", title);
    return 0;
}

int start_frame() {
    __disp_clear();
    return 0;
}

int end_frame() {
    __disp_render();
    return 0;
}

int draw_background() {
    __disp_set_color(0.2, 0.2, 0.4);
    __disp_fill_rect(0, 0, 800, 600);
    __disp_set_color(1, 1, 1);
    for (int i = 0; i < 50; i++) {
    int star_x = ((i * 37) % 800);
    int star_y = ((i * 73) % 600);
    __disp_fill_circle(star_x, star_y, 1);
    }
    return 0;
}

int init_player() {
    int player_x = 400;
    int player_y = 300;
    int player_vx = 2;
    int player_vy = 1.5;
    printf("%s\n", "Player initialized\n");
    return 0;
}

int update_player() {
    int player_x = (player_x + player_vx);
    int player_y = (player_y + player_vy);
    if (((player_x < 20) || (player_x > 780))) {
    int player_vx = (0 - player_vx);
    }
    if (((player_y < 20) || (player_y > 580))) {
    int player_vy = (0 - player_vy);
    }
    return 0;
}

int render_player() {
    __disp_set_color(0, 0.5, 1);
    __disp_fill_circle(player_x, player_y, 15);
    __disp_set_color(1, 1, 1);
    __disp_draw_circle(player_x, player_y, 15);
    return 0;
}

int init_enemies() {
    int enemy1_x = 100;
    int enemy1_y = 100;
    int enemy2_x = 600;
    int enemy2_y = 400;
    int enemy3_x = 300;
    int enemy3_y = 200;
    printf("%s\n", "Enemies initialized\n");
    return 0;
}

int update_enemies() {
    int enemy1_x = (enemy1_x + 3);
    if ((enemy1_x > 800)) {
    enemy1_x = -(30);
    }
    int enemy2_y = (enemy2_y - 2);
    if ((enemy2_y < -(30))) {
    enemy2_y = 630;
    }
    int enemy3_x = (400 + (150 * cos((enemy1_x * 0.02))));
    int enemy3_y = (300 + (100 * sin((enemy1_x * 0.02))));
    return 0;
}

int render_enemies() {
    __disp_set_color(1, 0, 0);
    __disp_fill_rect(enemy1_x, enemy1_y, 25, 25);
    __disp_set_color(1, 0.5, 0);
    __disp_draw_triangle(enemy2_x, enemy2_y, (enemy2_x - 15), (enemy2_y + 20), (enemy2_x + 15), (enemy2_y + 20));
    __disp_set_color(0.8, 0, 0.8);
    __disp_draw_triangle(enemy3_x, (enemy3_y - 12), (enemy3_x - 12), enemy3_y, (enemy3_x + 12), enemy3_y);
    __disp_draw_triangle(enemy3_x, (enemy3_y + 12), (enemy3_x - 12), enemy3_y, (enemy3_x + 12), enemy3_y);
    return 0;
}


int main(int argc, char** argv) {
    __disp_graphics_init();
    float player_x = 400;
    float player_y = 300;
    float player_vx = 0;
    float player_vy = 0;
    float enemy1_x = 100;
    float enemy1_y = 100;
    float enemy2_x = 600;
    float enemy2_y = 400;
    float enemy3_x = 300;
    float enemy3_y = 200;
    printf("%s\n", "=== Multi-File Game Project ===\n");
    int _ = init_engine("Multi-File Game", 800, 600);
    _ = init_player();
    _ = init_enemies();
    printf("%s\n", "Starting multi-file game...\n");
    for (int frame = 0; frame < 500; frame++) {
    _ = start_frame();
    _ = update_player();
    _ = update_enemies();
    _ = render_player();
    _ = render_enemies();
    _ = end_frame();
    }
    printf("%s\n", "Multi-file game complete!\n");
    __disp_graphics_cleanup();
    return 0;
}
