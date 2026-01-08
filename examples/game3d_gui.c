/* Displexity 3D Game Demo - GUI Version (Windows)
 * Compile: gcc -o game3d_gui game3d_gui.c -lgdi32 -luser32 -lm
 * Controls: WASD=move, QE=up/down, Mouse=look, ESC=quit
 */
#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Key codes */
#define KEY_NONE 0
#define KEY_UP 256
#define KEY_DOWN 257
#define KEY_LEFT 258
#define KEY_RIGHT 259
#define KEY_ESC 27

typedef struct { float x, y, z; } Vec3;
typedef struct { Vec3 v[3]; int color; } Triangle;
typedef struct { float m[4][4]; } Mat4;
typedef struct { Triangle* tris; int count; } Mesh;

typedef struct {
    int width, height;
    HWND hwnd;
    HDC hdc;
    unsigned int* frameBuffer;
    float* depthBuffer;
    int running;
    int mouseX, mouseY;
    int lastMouseX, lastMouseY;
    int mouseCapture;
} GuiRenderer;

typedef struct {
    float x, y, z;
    float yaw, pitch;
} Camera;

static GuiRenderer* g_renderer = NULL;

/* Vector math */
Vec3 vec3_sub(Vec3 a, Vec3 b) { return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z}; }
float vec3_dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 vec3_cross(Vec3 a, Vec3 b) { return (Vec3){a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x}; }
float vec3_len(Vec3 v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }
Vec3 vec3_norm(Vec3 v) { float l=vec3_len(v); return l>0?(Vec3){v.x/l,v.y/l,v.z/l}:(Vec3){0,0,0}; }

/* Matrix math */
Mat4 mat4_identity() { Mat4 m={{{0}}}; m.m[0][0]=m.m[1][1]=m.m[2][2]=m.m[3][3]=1.0f; return m; }

Mat4 mat4_rotY(float a) { 
    Mat4 m=mat4_identity(); 
    m.m[0][0]=cosf(a); m.m[0][2]=sinf(a); 
    m.m[2][0]=-sinf(a); m.m[2][2]=cosf(a); 
    return m; 
}

Mat4 mat4_rotX(float a) { 
    Mat4 m=mat4_identity(); 
    m.m[1][1]=cosf(a); m.m[1][2]=sinf(a); 
    m.m[2][1]=-sinf(a); m.m[2][2]=cosf(a); 
    return m; 
}

Mat4 mat4_trans(float x, float y, float z) { 
    Mat4 m=mat4_identity(); 
    m.m[3][0]=x; m.m[3][1]=y; m.m[3][2]=z; 
    return m; 
}

Mat4 mat4_proj(float fov, float aspect, float znear, float zfar) {
    Mat4 m={{{0}}}; 
    float f=1.0f/tanf(fov*0.5f*3.14159f/180.0f);
    m.m[0][0]=aspect*f; m.m[1][1]=f; 
    m.m[2][2]=zfar/(zfar-znear); 
    m.m[3][2]=(-zfar*znear)/(zfar-znear); 
    m.m[2][3]=1.0f;
    return m;
}

Vec3 mat4_mulv(Mat4* m, Vec3 v) {
    Vec3 o; 
    o.x=v.x*m->m[0][0]+v.y*m->m[1][0]+v.z*m->m[2][0]+m->m[3][0];
    o.y=v.x*m->m[0][1]+v.y*m->m[1][1]+v.z*m->m[2][1]+m->m[3][1];
    o.z=v.x*m->m[0][2]+v.y*m->m[1][2]+v.z*m->m[2][2]+m->m[3][2];
    float w=v.x*m->m[0][3]+v.y*m->m[1][3]+v.z*m->m[2][3]+m->m[3][3];
    if(w!=0.0f){o.x/=w;o.y/=w;o.z/=w;} 
    return o;
}

Mat4 mat4_mul(Mat4* a, Mat4* b) {
    Mat4 r={{{0}}}; 
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            for(int k=0;k<4;k++)
                r.m[i][j]+=a->m[i][k]*b->m[k][j]; 
    return r;
}

/* Window procedure */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_DESTROY:
            if(g_renderer) g_renderer->running = 0;
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE) {
                if(g_renderer) g_renderer->running = 0;
                DestroyWindow(hwnd);
            }
            return 0;
        case WM_MOUSEMOVE:
            if(g_renderer) {
                g_renderer->mouseX = LOWORD(lParam);
                g_renderer->mouseY = HIWORD(lParam);
            }
            return 0;
        case WM_LBUTTONDOWN:
            if(g_renderer && !g_renderer->mouseCapture) {
                SetCapture(hwnd);
                ShowCursor(FALSE);
                g_renderer->mouseCapture = 1;
                g_renderer->lastMouseX = g_renderer->mouseX;
                g_renderer->lastMouseY = g_renderer->mouseY;
            }
            return 0;
        case WM_RBUTTONDOWN:
            if(g_renderer && g_renderer->mouseCapture) {
                ReleaseCapture();
                ShowCursor(TRUE);
                g_renderer->mouseCapture = 0;
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* GUI Renderer */
GuiRenderer* gui_create(int w, int h, const char* title) {
    GuiRenderer* r = (GuiRenderer*)malloc(sizeof(GuiRenderer));
    r->width = w; r->height = h; r->running = 1;
    r->mouseCapture = 0;
    r->frameBuffer = (unsigned int*)malloc(w * h * sizeof(unsigned int));
    r->depthBuffer = (float*)malloc(w * h * sizeof(float));
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "Disp3D";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);
    
    RECT rect = {0, 0, w, h};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    r->hwnd = CreateWindow("Disp3D", title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    r->hdc = GetDC(r->hwnd);
    g_renderer = r;
    ShowWindow(r->hwnd, SW_SHOW);
    return r;
}

void gui_destroy(GuiRenderer* r) {
    if(r->mouseCapture) {
        ReleaseCapture();
        ShowCursor(TRUE);
    }
    ReleaseDC(r->hwnd, r->hdc);
    free(r->frameBuffer);
    free(r->depthBuffer);
    free(r);
}

void gui_clear(GuiRenderer* r, unsigned int color) {
    int sz = r->width * r->height;
    for(int i = 0; i < sz; i++) {
        r->frameBuffer[i] = color;
        r->depthBuffer[i] = 0.0f;
    }
}

void gui_setpixel(GuiRenderer* r, int x, int y, unsigned int color, float d) {
    if(x < 0 || x >= r->width || y < 0 || y >= r->height) return;
    int i = y * r->width + x;
    if(d > r->depthBuffer[i]) {
        r->frameBuffer[i] = color;
        r->depthBuffer[i] = d;
    }
}

void gui_present(GuiRenderer* r) {
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = r->width;
    bmi.bmiHeader.biHeight = -r->height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    SetDIBitsToDevice(r->hdc, 0, 0, r->width, r->height, 0, 0, 0, r->height,
                      r->frameBuffer, &bmi, DIB_RGB_COLORS);
}

int gui_process(GuiRenderer* r) {
    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return r->running;
}

void gui_getmousedelta(GuiRenderer* r, int* dx, int* dy) {
    if(r->mouseCapture) {
        *dx = r->mouseX - r->lastMouseX;
        *dy = r->mouseY - r->lastMouseY;
        r->lastMouseX = r->mouseX;
        r->lastMouseY = r->mouseY;
        /* Re-center mouse */
        POINT center = {r->width / 2, r->height / 2};
        ClientToScreen(r->hwnd, &center);
        SetCursorPos(center.x, center.y);
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(r->hwnd, &pt);
        r->lastMouseX = pt.x;
        r->lastMouseY = pt.y;
    } else {
        *dx = *dy = 0;
    }
}

/* Triangle fill */
void gui_filltri(GuiRenderer* r, int x0, int y0, float z0, int x1, int y1, float z1,
                 int x2, int y2, float z2, unsigned int color) {
    int tmp;
    if(y0 > y1) { tmp=x0;x0=x1;x1=tmp; tmp=y0;y0=y1;y1=tmp; float t=z0;z0=z1;z1=t; }
    if(y0 > y2) { tmp=x0;x0=x2;x2=tmp; tmp=y0;y0=y2;y2=tmp; float t=z0;z0=z2;z2=t; }
    if(y1 > y2) { tmp=x1;x1=x2;x2=tmp; tmp=y1;y1=y2;y2=tmp; float t=z1;z1=z2;z2=t; }
    int th = y2 - y0; if(th == 0) return;
    for(int y = y0; y <= y2; y++) {
        int sh = (y > y1 || y1 == y0);
        int seg = sh ? y2 - y1 : y1 - y0;
        if(seg == 0) continue;
        float a = (float)(y - y0) / th;
        float b = sh ? (float)(y - y1) / seg : (float)(y - y0) / seg;
        int ax = x0 + (int)((x2 - x0) * a);
        int bx = sh ? x1 + (int)((x2 - x1) * b) : x0 + (int)((x1 - x0) * b);
        float az = z0 + (z2 - z0) * a;
        float bz = sh ? z1 + (z2 - z1) * b : z0 + (z1 - z0) * b;
        if(ax > bx) { tmp=ax;ax=bx;bx=tmp; float t=az;az=bz;bz=t; }
        for(int x = ax; x <= bx; x++) {
            float t = (bx == ax) ? 0 : (float)(x - ax) / (bx - ax);
            gui_setpixel(r, x, y, color, az + (bz - az) * t);
        }
    }
}

/* Mesh creation */
Mesh mesh_cube() {
    Mesh m; m.count = 12;
    m.tris = (Triangle*)malloc(12 * sizeof(Triangle));
    Vec3 v[8] = {{0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1}};
    int f[12][4] = {
        {0,3,2,1},{1,2,6,2},{5,6,7,3},{4,7,3,4},{3,7,6,5},{4,0,1,6},
        {0,2,1,1},{1,6,5,2},{5,7,4,3},{4,3,0,4},{3,6,2,5},{4,1,5,6}
    };
    for(int i = 0; i < 12; i++) {
        m.tris[i].v[0] = v[f[i][0]];
        m.tris[i].v[1] = v[f[i][1]];
        m.tris[i].v[2] = v[f[i][2]];
        m.tris[i].color = f[i][3];
    }
    return m;
}

Mesh mesh_pyramid() {
    Mesh m; m.count = 6;
    m.tris = (Triangle*)malloc(6 * sizeof(Triangle));
    Vec3 apex = {0.5f, 1.0f, 0.5f};
    m.tris[0] = (Triangle){{(Vec3){0,0,0}, apex, (Vec3){1,0,0}}, 1};
    m.tris[1] = (Triangle){{(Vec3){1,0,0}, apex, (Vec3){1,0,1}}, 2};
    m.tris[2] = (Triangle){{(Vec3){1,0,1}, apex, (Vec3){0,0,1}}, 3};
    m.tris[3] = (Triangle){{(Vec3){0,0,1}, apex, (Vec3){0,0,0}}, 4};
    m.tris[4] = (Triangle){{(Vec3){0,0,0}, (Vec3){1,0,0}, (Vec3){1,0,1}}, 5};
    m.tris[5] = (Triangle){{(Vec3){0,0,0}, (Vec3){1,0,1}, (Vec3){0,0,1}}, 5};
    return m;
}

/* Render mesh */
void gui_render(GuiRenderer* r, Mesh* mesh, Mat4* transform, Mat4* proj) {
    Vec3 light = {0, 0, -1};
    light = vec3_norm(light);
    unsigned int colors[] = {0x404040, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF};
    
    for(int i = 0; i < mesh->count; i++) {
        Triangle* tri = &mesh->tris[i];
        Vec3 v0 = mat4_mulv(transform, tri->v[0]);
        Vec3 v1 = mat4_mulv(transform, tri->v[1]);
        Vec3 v2 = mat4_mulv(transform, tri->v[2]);
        Vec3 n = vec3_norm(vec3_cross(vec3_sub(v1, v0), vec3_sub(v2, v0)));
        if(vec3_dot(n, v0) >= 0) continue;
        float dp = vec3_dot(n, light);
        if(dp < 0.1f) dp = 0.1f;
        
        Vec3 p0 = mat4_mulv(proj, v0);
        Vec3 p1 = mat4_mulv(proj, v1);
        Vec3 p2 = mat4_mulv(proj, v2);
        int sx0 = (int)((p0.x + 1) * 0.5f * r->width);
        int sy0 = (int)((p0.y + 1) * 0.5f * r->height);
        int sx1 = (int)((p1.x + 1) * 0.5f * r->width);
        int sy1 = (int)((p1.y + 1) * 0.5f * r->height);
        int sx2 = (int)((p2.x + 1) * 0.5f * r->width);
        int sy2 = (int)((p2.y + 1) * 0.5f * r->height);
        float avgZ = (p0.z + p1.z + p2.z) / 3.0f;
        
        unsigned int baseColor = colors[tri->color % 7];
        unsigned int cr = (unsigned int)(((baseColor >> 16) & 0xFF) * dp);
        unsigned int cg = (unsigned int)(((baseColor >> 8) & 0xFF) * dp);
        unsigned int cb = (unsigned int)((baseColor & 0xFF) * dp);
        unsigned int shadedColor = (cr << 16) | (cg << 8) | cb;
        
        gui_filltri(r, sx0, sy0, avgZ, sx1, sy1, avgZ, sx2, sy2, avgZ, shadedColor);
    }
}

/* Draw text */
void gui_text(GuiRenderer* r, int x, int y, const char* text, unsigned int color) {
    SetBkMode(r->hdc, TRANSPARENT);
    SetTextColor(r->hdc, RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF));
    TextOutA(r->hdc, x, y, text, (int)strlen(text));
}

/* Main */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;
    
    GuiRenderer* r = gui_create(800, 600, "Displexity 3D Demo - Click to capture mouse, Right-click to release");
    
    Mesh cube = mesh_cube();
    Mesh pyramid = mesh_pyramid();
    Mat4 proj = mat4_proj(90.0f, (float)r->height / r->width, 0.1f, 1000.0f);
    
    Camera cam = {0, 0, -5, 0, 0};
    float cubeAngle = 0.0f;
    float pyramidAngle = 0.0f;
    
    while(gui_process(r)) {
        /* Handle mouse look */
        int dx, dy;
        gui_getmousedelta(r, &dx, &dy);
        if(r->mouseCapture) {
            cam.yaw += dx * 0.003f;
            cam.pitch += dy * 0.003f;
            if(cam.pitch > 1.5f) cam.pitch = 1.5f;
            if(cam.pitch < -1.5f) cam.pitch = -1.5f;
        }
        
        /* Handle keyboard */
        float speed = 0.1f;
        float fwdX = sinf(cam.yaw) * speed;
        float fwdZ = cosf(cam.yaw) * speed;
        
        if(GetAsyncKeyState('W') & 0x8000) { cam.x += fwdX; cam.z += fwdZ; }
        if(GetAsyncKeyState('S') & 0x8000) { cam.x -= fwdX; cam.z -= fwdZ; }
        if(GetAsyncKeyState('A') & 0x8000) { cam.x -= fwdZ; cam.z += fwdX; }
        if(GetAsyncKeyState('D') & 0x8000) { cam.x += fwdZ; cam.z -= fwdX; }
        if(GetAsyncKeyState('E') & 0x8000) cam.y += speed;
        if(GetAsyncKeyState('Q') & 0x8000) cam.y -= speed;
        
        /* Arrow keys for rotation when mouse not captured */
        if(!r->mouseCapture) {
            if(GetAsyncKeyState(VK_LEFT) & 0x8000) cam.yaw -= 0.05f;
            if(GetAsyncKeyState(VK_RIGHT) & 0x8000) cam.yaw += 0.05f;
            if(GetAsyncKeyState(VK_UP) & 0x8000) cam.pitch -= 0.05f;
            if(GetAsyncKeyState(VK_DOWN) & 0x8000) cam.pitch += 0.05f;
        }
        
        gui_clear(r, 0x202040);
        
        /* Camera view matrix */
        Mat4 camRotY = mat4_rotY(-cam.yaw);
        Mat4 camRotX = mat4_rotX(-cam.pitch);
        Mat4 camTrans = mat4_trans(-cam.x, -cam.y, -cam.z);
        Mat4 view = mat4_mul(&camTrans, &camRotY);
        view = mat4_mul(&view, &camRotX);
        
        /* Render cube */
        Mat4 cubeRotX = mat4_rotX(cubeAngle * 0.5f);
        Mat4 cubeRotY = mat4_rotY(cubeAngle);
        Mat4 cubeTrans = mat4_trans(-0.5f, -0.5f, 3.0f);
        Mat4 cubeWorld = mat4_mul(&cubeRotX, &cubeRotY);
        cubeWorld = mat4_mul(&cubeWorld, &cubeTrans);
        cubeWorld = mat4_mul(&cubeWorld, &view);
        gui_render(r, &cube, &cubeWorld, &proj);
        
        /* Render pyramid */
        Mat4 pyrRotY = mat4_rotY(-pyramidAngle);
        Mat4 pyrTrans = mat4_trans(2.0f, -0.5f, 5.0f);
        Mat4 pyrWorld = mat4_mul(&pyrRotY, &pyrTrans);
        pyrWorld = mat4_mul(&pyrWorld, &view);
        gui_render(r, &pyramid, &pyrWorld, &proj);
        
        /* Render second cube */
        Mat4 cube2Trans = mat4_trans(-3.0f, 0.0f, 6.0f);
        Mat4 cube2RotY = mat4_rotY(cubeAngle * 0.7f);
        Mat4 cube2World = mat4_mul(&cube2RotY, &cube2Trans);
        cube2World = mat4_mul(&cube2World, &view);
        gui_render(r, &cube, &cube2World, &proj);
        
        gui_present(r);
        
        /* Draw HUD (after present for GDI text) */
        char buf[128];
        gui_text(r, 10, 10, "Displexity 3D Demo", 0xFFFFFF);
        sprintf(buf, "Pos: %.1f, %.1f, %.1f", cam.x, cam.y, cam.z);
        gui_text(r, 10, 30, buf, 0xCCCCCC);
        gui_text(r, 10, r->height - 30, "WASD=Move QE=Up/Down Mouse=Look ESC=Quit", 0x888888);
        if(!r->mouseCapture) {
            gui_text(r, 10, 50, "Click to capture mouse", 0xFFFF00);
        }
        
        cubeAngle += 0.02f;
        pyramidAngle += 0.015f;
        Sleep(16);
    }
    
    gui_destroy(r);
    free(cube.tris);
    free(pyramid.tris);
    return 0;
}
#else
#include <stdio.h>
int main() {
    printf("GUI version requires Windows. Use game3d_tui.c for cross-platform TUI version.\n");
    return 1;
}
#endif
