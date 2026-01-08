/* Displexity 3D Game Demo - TUI Version
 * Compile: gcc -o game3d_tui game3d_tui.c -lm
 * Controls: WASD=move, QE=up/down, Arrows=rotate, ESC=quit
 */
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

/* Camera */
typedef struct {
    float x, y, z;
    float yaw, pitch;
} Camera;

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

/* TUI Renderer */
TuiRenderer* tui_create(int w, int h) {
    TuiRenderer* r=(TuiRenderer*)malloc(sizeof(TuiRenderer)); 
    r->width=w; r->height=h; r->running=1;
    int sz=w*h; 
    r->frontBuffer=(char*)malloc(sz); 
    r->backBuffer=(char*)malloc(sz);
    r->depthBuffer=(float*)malloc(sz*sizeof(float)); 
    r->colorBuffer=(int*)malloc(sz*sizeof(int));
    memset(r->frontBuffer,' ',sz); 
    memset(r->backBuffer,' ',sz); 
    return r;
}

void tui_destroy(TuiRenderer* r) { 
    free(r->frontBuffer); free(r->backBuffer); 
    free(r->depthBuffer); free(r->colorBuffer); 
    free(r); 
}

void tui_init(TuiRenderer* r) {
#ifdef _WIN32
    r->hConsole=GetStdHandle(STD_OUTPUT_HANDLE); 
    r->hConsoleIn=GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(r->hConsoleIn,&r->oldMode); 
    SetConsoleMode(r->hConsoleIn,ENABLE_EXTENDED_FLAGS);
    CONSOLE_CURSOR_INFO ci; 
    GetConsoleCursorInfo(r->hConsole,&ci); 
    ci.bVisible=FALSE; 
    SetConsoleCursorInfo(r->hConsole,&ci);
#else
    tcgetattr(STDIN_FILENO,&r->oldTermios); 
    struct termios nt=r->oldTermios;
    nt.c_lflag&=~(ICANON|ECHO); 
    nt.c_cc[VMIN]=0; 
    nt.c_cc[VTIME]=0;
    tcsetattr(STDIN_FILENO,TCSANOW,&nt); 
    printf("\033[?25l");
#endif
}

void tui_cleanup(TuiRenderer* r) {
#ifdef _WIN32
    SetConsoleMode(r->hConsoleIn,r->oldMode); 
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(r->hConsole,&ci); 
    ci.bVisible=TRUE; 
    SetConsoleCursorInfo(r->hConsole,&ci);
    SetConsoleTextAttribute(r->hConsole,7);
#else
    tcsetattr(STDIN_FILENO,TCSANOW,&r->oldTermios); 
    printf("\033[?25h\033[0m\n");
#endif
}

void tui_clear(TuiRenderer* r) {
    int sz=r->width*r->height; 
    memset(r->backBuffer,' ',sz);
    for(int i=0;i<sz;i++){
        r->depthBuffer[i]=0.0f;
        r->colorBuffer[i]=7;
    }
}

void tui_setpixel(TuiRenderer* r, int x, int y, char c, float d, int col) {
    if(x<0||x>=r->width||y<0||y>=r->height)return;
    int i=y*r->width+x; 
    if(d>r->depthBuffer[i]){
        r->backBuffer[i]=c;
        r->depthBuffer[i]=d;
        r->colorBuffer[i]=col;
    }
}

void tui_present(TuiRenderer* r) {
#ifdef _WIN32
    COORD pos={0,0}; 
    SetConsoleCursorPosition(r->hConsole,pos);
    int lastCol=-1;
    for(int y=0;y<r->height;y++){
        for(int x=0;x<r->width;x++){
            int i=y*r->width+x;
            if(r->colorBuffer[i]!=lastCol){
                SetConsoleTextAttribute(r->hConsole,r->colorBuffer[i]);
                lastCol=r->colorBuffer[i];
            }
            putchar(r->backBuffer[i]);
        }
        if(y<r->height-1)putchar('\n');
    }
#else
    printf("\033[H"); 
    int lastCol=-1;
    for(int y=0;y<r->height;y++){
        for(int x=0;x<r->width;x++){
            int i=y*r->width+x;
            if(r->colorBuffer[i]!=lastCol){
                printf("\033[%d;%dm",30+(r->colorBuffer[i]&7),40);
                lastCol=r->colorBuffer[i];
            }
            putchar(r->backBuffer[i]);
        }
        if(y<r->height-1)putchar('\n');
    }
#endif
    fflush(stdout); 
    char* tmp=r->frontBuffer; 
    r->frontBuffer=r->backBuffer; 
    r->backBuffer=tmp;
}

int tui_getkey(TuiRenderer* r) {
    (void)r;
#ifdef _WIN32
    if(_kbhit()){
        int ch=_getch();
        if(ch==0||ch==224){
            ch=_getch();
            switch(ch){
                case 72:return KEY_UP;
                case 80:return KEY_DOWN;
                case 75:return KEY_LEFT;
                case 77:return KEY_RIGHT;
            }
            return ch+256;
        }
        return ch;
    }
    return 0;
#else
    int ch=getchar(); 
    if(ch==EOF||ch==-1)return 0;
    if(ch==27){
        int c2=getchar();
        if(c2=='['){
            int c3=getchar();
            switch(c3){
                case 'A':return KEY_UP;
                case 'B':return KEY_DOWN;
                case 'D':return KEY_LEFT;
                case 'C':return KEY_RIGHT;
            }
        }
        return 27;
    }
    return ch;
#endif
}

/* Mesh creation */
Mesh mesh_cube() {
    Mesh m; m.count=12; 
    m.tris=(Triangle*)malloc(12*sizeof(Triangle));
    Vec3 v[8]={{0,0,0},{1,0,0},{1,1,0},{0,1,0},{0,0,1},{1,0,1},{1,1,1},{0,1,1}};
    int f[12][4]={
        {0,3,2,1},{1,2,6,2},{5,6,7,3},{4,7,3,4},{3,7,6,5},{4,0,1,6},
        {0,2,1,1},{1,6,5,2},{5,7,4,3},{4,3,0,4},{3,6,2,5},{4,1,5,6}
    };
    for(int i=0;i<12;i++){
        m.tris[i].v[0]=v[f[i][0]];
        m.tris[i].v[1]=v[f[i][1]];
        m.tris[i].v[2]=v[f[i][2]];
        m.tris[i].color=f[i][3];
    }
    return m;
}

Mesh mesh_pyramid() {
    Mesh m; m.count=6;
    m.tris=(Triangle*)malloc(6*sizeof(Triangle));
    Vec3 apex={0.5f,1.0f,0.5f};
    m.tris[0]=(Triangle){{(Vec3){0,0,0},apex,(Vec3){1,0,0}},1};
    m.tris[1]=(Triangle){{(Vec3){1,0,0},apex,(Vec3){1,0,1}},2};
    m.tris[2]=(Triangle){{(Vec3){1,0,1},apex,(Vec3){0,0,1}},3};
    m.tris[3]=(Triangle){{(Vec3){0,0,1},apex,(Vec3){0,0,0}},4};
    m.tris[4]=(Triangle){{(Vec3){0,0,0},(Vec3){1,0,0},(Vec3){1,0,1}},5};
    m.tris[5]=(Triangle){{(Vec3){0,0,0},(Vec3){1,0,1},(Vec3){0,0,1}},5};
    return m;
}

/* Triangle fill */
void tui_filltri(TuiRenderer* r, int x0, int y0, float z0, int x1, int y1, float z1, 
                 int x2, int y2, float z2, char c, int col) {
    int tmp; 
    if(y0>y1){tmp=x0;x0=x1;x1=tmp;tmp=y0;y0=y1;y1=tmp;float t=z0;z0=z1;z1=t;}
    if(y0>y2){tmp=x0;x0=x2;x2=tmp;tmp=y0;y0=y2;y2=tmp;float t=z0;z0=z2;z2=t;}
    if(y1>y2){tmp=x1;x1=x2;x2=tmp;tmp=y1;y1=y2;y2=tmp;float t=z1;z1=z2;z2=t;}
    int th=y2-y0; if(th==0)return;
    for(int y=y0;y<=y2;y++){
        int sh=(y>y1||y1==y0);
        int seg=sh?y2-y1:y1-y0;
        if(seg==0)continue;
        float a=(float)(y-y0)/th; 
        float b=sh?(float)(y-y1)/seg:(float)(y-y0)/seg;
        int ax=x0+(int)((x2-x0)*a); 
        int bx=sh?x1+(int)((x2-x1)*b):x0+(int)((x1-x0)*b);
        float az=z0+(z2-z0)*a; 
        float bz=sh?z1+(z2-z1)*b:z0+(z1-z0)*b;
        if(ax>bx){tmp=ax;ax=bx;bx=tmp;float t=az;az=bz;bz=t;}
        for(int x=ax;x<=bx;x++){
            float t=(bx==ax)?0:(float)(x-ax)/(bx-ax);
            tui_setpixel(r,x,y,c,az+(bz-az)*t,col);
        }
    }
}

/* Render mesh */
void tui_render(TuiRenderer* r, Mesh* mesh, Mat4* transform, Mat4* proj) {
    const char* shade=" .:-=+*#%@"; 
    Vec3 light={0,0,-1}; 
    light=vec3_norm(light);
    
    for(int i=0;i<mesh->count;i++){
        Triangle* tri=&mesh->tris[i];
        Vec3 v0=mat4_mulv(transform,tri->v[0]); 
        Vec3 v1=mat4_mulv(transform,tri->v[1]); 
        Vec3 v2=mat4_mulv(transform,tri->v[2]);
        Vec3 n=vec3_norm(vec3_cross(vec3_sub(v1,v0),vec3_sub(v2,v0)));
        if(vec3_dot(n,v0)>=0)continue; 
        float dp=vec3_dot(n,light); 
        if(dp<0.1f)dp=0.1f;
        int si=(int)(dp*9); 
        if(si>9)si=9;
        Vec3 p0=mat4_mulv(proj,v0);
        Vec3 p1=mat4_mulv(proj,v1);
        Vec3 p2=mat4_mulv(proj,v2);
        int sx0=(int)((p0.x+1)*0.5f*r->width);
        int sy0=(int)((p0.y+1)*0.5f*r->height);
        int sx1=(int)((p1.x+1)*0.5f*r->width);
        int sy1=(int)((p1.y+1)*0.5f*r->height);
        int sx2=(int)((p2.x+1)*0.5f*r->width);
        int sy2=(int)((p2.y+1)*0.5f*r->height);
        float avgZ=(p0.z+p1.z+p2.z)/3.0f;
        tui_filltri(r,sx0,sy0,avgZ,sx1,sy1,avgZ,sx2,sy2,avgZ,shade[si],tri->color);
    }
}

/* Draw text */
void tui_text(TuiRenderer* r, int x, int y, const char* text, int col) {
    while(*text){
        tui_setpixel(r,x++,y,*text++,1.0f,col);
    }
}

/* Main */
int main() {
    TuiRenderer* r = tui_create(80, 24);
    tui_init(r);
    
    Mesh cube = mesh_cube();
    Mesh pyramid = mesh_pyramid();
    Mat4 proj = mat4_proj(90.0f, (float)r->height / r->width * 2.0f, 0.1f, 1000.0f);
    
    Camera cam = {0, 0, -5, 0, 0};
    float cubeAngle = 0.0f;
    float pyramidAngle = 0.0f;
    
    while (r->running) {
        int key = tui_getkey(r);
        
        /* Handle input */
        if (key == KEY_ESC || key == 'q') r->running = 0;
        
        /* Camera movement */
        float speed = 0.15f;
        float fwdX = sinf(cam.yaw) * speed;
        float fwdZ = cosf(cam.yaw) * speed;
        
        if (key == 'w') { cam.x += fwdX; cam.z += fwdZ; }
        if (key == 's') { cam.x -= fwdX; cam.z -= fwdZ; }
        if (key == 'a') { cam.x -= fwdZ; cam.z += fwdX; }
        if (key == 'd') { cam.x += fwdZ; cam.z -= fwdX; }
        if (key == 'e') cam.y += speed;
        if (key == 'r') cam.y -= speed;
        
        /* Camera rotation */
        if (key == KEY_LEFT) cam.yaw -= 0.1f;
        if (key == KEY_RIGHT) cam.yaw += 0.1f;
        if (key == KEY_UP) cam.pitch -= 0.1f;
        if (key == KEY_DOWN) cam.pitch += 0.1f;
        
        /* Clamp pitch */
        if (cam.pitch > 1.5f) cam.pitch = 1.5f;
        if (cam.pitch < -1.5f) cam.pitch = -1.5f;
        
        tui_clear(r);
        
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
        tui_render(r, &cube, &cubeWorld, &proj);
        
        /* Render pyramid */
        Mat4 pyrRotY = mat4_rotY(-pyramidAngle);
        Mat4 pyrTrans = mat4_trans(2.0f, -0.5f, 5.0f);
        Mat4 pyrWorld = mat4_mul(&pyrRotY, &pyrTrans);
        pyrWorld = mat4_mul(&pyrWorld, &view);
        tui_render(r, &pyramid, &pyrWorld, &proj);
        
        /* Render second cube */
        Mat4 cube2Trans = mat4_trans(-3.0f, 0.0f, 6.0f);
        Mat4 cube2RotY = mat4_rotY(cubeAngle * 0.7f);
        Mat4 cube2World = mat4_mul(&cube2RotY, &cube2Trans);
        cube2World = mat4_mul(&cube2World, &view);
        tui_render(r, &cube, &cube2World, &proj);
        
        /* Draw HUD */
        char buf[64];
        tui_text(r, 1, 0, "Displexity 3D Demo", 15);
        sprintf(buf, "Pos: %.1f,%.1f,%.1f", cam.x, cam.y, cam.z);
        tui_text(r, 1, 1, buf, 7);
        tui_text(r, 1, r->height-1, "WASD=Move ER=Up/Down Arrows=Look Q=Quit", 8);
        
        tui_present(r);
        
        cubeAngle += 0.03f;
        pyramidAngle += 0.02f;
        SLEEP(16);
    }
    
    tui_cleanup(r);
    tui_destroy(r);
    free(cube.tris);
    free(pyramid.tris);
    return 0;
}
