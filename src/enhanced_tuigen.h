#ifndef DISPLEXITY_ENHANCED_TUIGEN_H
#define DISPLEXITY_ENHANCED_TUIGEN_H

#include "tuigen.h"
#include "enhanced_tui_renderer.h"

namespace disp {

class EnhancedTuiGenerator : public TuiGenerator {
private:
    bool needsEnhancedRenderer = false;
    bool needsSoundSystem = false;
    bool needsVoxelWorld = false;
    bool needsRaytracing = false;
    
public:
    std::vector<uint8_t> generateEnhanced(const Program& prog) {
        // Scan for enhanced features
        scanEnhancedFeatures(prog);
        
        // Generate standard TUI bytecode
        return generate(prog);
    }
    
    std::string generateEnhancedC(const Program& prog) {
        scanEnhancedFeatures(prog);
        
        std::stringstream code;
        
        // Emit enhanced includes
        code << "#include <stdio.h>\n";
        code << "#include <stdlib.h>\n";
        code << "#include <string.h>\n";
        code << "#include <math.h>\n";
        code << "#include <time.h>\n";
        
        if (needsEnhancedRenderer) {
            code << "#ifdef _WIN32\n";
            code << "#define WIN32_LEAN_AND_MEAN\n";
            code << "#include <windows.h>\n";
            code << "#include <conio.h>\n";
            code << "#include <mmsystem.h>\n";
            code << "#pragma comment(lib, \"winmm.lib\")\n";
            code << "#define SLEEP(ms) Sleep(ms)\n";
            code << "#else\n";
            code << "#include <termios.h>\n";
            code << "#include <unistd.h>\n";
            code << "#include <sys/ioctl.h>\n";
            code << "#include <pthread.h>\n";
            code << "#define SLEEP(ms) usleep((ms) * 1000)\n";
            code << "#endif\n\n";
        }
        
        // Emit enhanced renderer code
        if (needsEnhancedRenderer) {
            code << emitEnhancedRenderer();
        }
        
        if (needsSoundSystem) {
            code << emitSoundSystem();
        }
        
        if (needsVoxelWorld) {
            code << emitVoxelWorld();
        }
        
        if (needsRaytracing) {
            code << emitRaytracing();
        }
        
        // Emit enhanced runtime functions
        code << emitEnhancedRuntimeFunctions();
        
        // Generate main program
        code << "int main() {\n";
        
        // Initialize enhanced systems
        if (needsEnhancedRenderer) {
            code << "    enhanced_tui_init(120, 40, 200.0f);\n";
        }
        
        if (needsSoundSystem) {
            code << "    sound_system_init();\n";
        }
        
        // Generate program statements
        for (const auto& stmt : prog.statements) {
            if (!std::dynamic_pointer_cast<FunctionDecl>(stmt)) {
                code << "    " << generateEnhancedStatement(stmt) << "\n";
            }
        }
        
        // Cleanup
        if (needsEnhancedRenderer) {
            code << "    enhanced_tui_cleanup();\n";
        }
        
        if (needsSoundSystem) {
            code << "    sound_system_cleanup();\n";
        }
        
        code << "    return 0;\n";
        code << "}\n";
        
        return code.str();
    }
    
private:
    void scanEnhancedFeatures(const Program& prog) {
        for (const auto& stmt : prog.statements) {
            scanStatementForFeatures(stmt);
        }
    }
    
    void scanStatementForFeatures(const StmtPtr& stmt) {
        if (auto funcCall = std::dynamic_pointer_cast<FunctionCall>(stmt)) {
            if (funcCall->name.find("enhanced_") == 0) {
                needsEnhancedRenderer = true;
            }
            if (funcCall->name.find("sound_") == 0 || 
                funcCall->name.find("load_sound") == 0 ||
                funcCall->name.find("play_sound") == 0) {
                needsSoundSystem = true;
            }
            if (funcCall->name.find("voxel") != std::string::npos ||
                funcCall->name.find("chunk") != std::string::npos) {
                needsVoxelWorld = true;
            }
            if (funcCall->name.find("raytracing") != std::string::npos ||
                funcCall->name.find("raycast") != std::string::npos) {
                needsRaytracing = true;
            }
        }
        
        // Recursively scan compound statements
        if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            for (const auto& s : ifStmt->thenBlock) scanStatementForFeatures(s);
            for (const auto& s : ifStmt->elseBlock) scanStatementForFeatures(s);
        }
        else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            for (const auto& s : whileStmt->body) scanStatementForFeatures(s);
        }
        else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            for (const auto& s : forStmt->body) scanStatementForFeatures(s);
        }
    }
    
    std::string emitEnhancedRenderer() {
        return R"(
/* Enhanced TUI Renderer with 200 FPS and Advanced Features */
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
    float targetFPS;
    clock_t lastFrame;
    int textureCount;
    unsigned int** textures;
    int* textureWidths;
    int* textureHeights;
#ifdef _WIN32
    HANDLE hConsole, hConsoleIn;
    DWORD oldMode;
#else
    struct termios oldTermios;
#endif
} EnhancedTuiRenderer;

static EnhancedTuiRenderer* g_renderer = NULL;
static const char* advancedShade = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

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
    m.m[0][0]=aspect*f; m.m[1][1]=f; m.m[2][2]=zfar/(zfar-znear); 
    m.m[3][2]=(-zfar*znear)/(zfar-znear); m.m[2][3]=1.0f; return m;
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

void enhanced_tui_init(int w, int h, float fps) {
    g_renderer = (EnhancedTuiRenderer*)malloc(sizeof(EnhancedTuiRenderer));
    g_renderer->width = w; g_renderer->height = h; g_renderer->running = 1;
    g_renderer->targetFPS = fps; g_renderer->lastFrame = clock();
    int sz = w * h;
    g_renderer->frontBuffer = (char*)malloc(sz);
    g_renderer->backBuffer = (char*)malloc(sz);
    g_renderer->depthBuffer = (float*)malloc(sz * sizeof(float));
    g_renderer->colorBuffer = (int*)malloc(sz * sizeof(int));
    g_renderer->textureCount = 0;
    g_renderer->textures = NULL;
    g_renderer->textureWidths = NULL;
    g_renderer->textureHeights = NULL;
    memset(g_renderer->frontBuffer, ' ', sz);
    memset(g_renderer->backBuffer, ' ', sz);
    
#ifdef _WIN32
    g_renderer->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    g_renderer->hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(g_renderer->hConsoleIn, &g_renderer->oldMode);
    SetConsoleMode(g_renderer->hConsoleIn, ENABLE_EXTENDED_FLAGS);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(g_renderer->hConsole, &ci);
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(g_renderer->hConsole, &ci);
#else
    tcgetattr(STDIN_FILENO, &g_renderer->oldTermios);
    struct termios nt = g_renderer->oldTermios;
    nt.c_lflag &= ~(ICANON | ECHO);
    nt.c_cc[VMIN] = 0; nt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &nt);
    printf("\033[?25l");
#endif
}

void enhanced_clear(unsigned int skyColor) {
    int sz = g_renderer->width * g_renderer->height;
    memset(g_renderer->backBuffer, ' ', sz);
    for(int i = 0; i < sz; i++) {
        g_renderer->depthBuffer[i] = 0.0f;
        g_renderer->colorBuffer[i] = 9; // Sky blue
    }
}

void enhanced_setpixel(int x, int y, char c, float d, int col) {
    if(x < 0 || x >= g_renderer->width || y < 0 || y >= g_renderer->height) return;
    int i = y * g_renderer->width + x;
    if(d > g_renderer->depthBuffer[i]) {
        g_renderer->backBuffer[i] = c;
        g_renderer->depthBuffer[i] = d;
        g_renderer->colorBuffer[i] = col;
    }
}

void enhanced_present() {
#ifdef _WIN32
    COORD pos = {0, 0};
    SetConsoleCursorPosition(g_renderer->hConsole, pos);
    int lastCol = -1;
    for(int y = 0; y < g_renderer->height; y++) {
        for(int x = 0; x < g_renderer->width; x++) {
            int i = y * g_renderer->width + x;
            if(g_renderer->colorBuffer[i] != lastCol) {
                SetConsoleTextAttribute(g_renderer->hConsole, g_renderer->colorBuffer[i]);
                lastCol = g_renderer->colorBuffer[i];
            }
            putchar(g_renderer->backBuffer[i]);
        }
        if(y < g_renderer->height - 1) putchar('\n');
    }
#else
    printf("\033[H");
    int lastCol = -1;
    for(int y = 0; y < g_renderer->height; y++) {
        for(int x = 0; x < g_renderer->width; x++) {
            int i = y * g_renderer->width + x;
            if(g_renderer->colorBuffer[i] != lastCol) {
                printf("\033[%d;%dm", 30 + (g_renderer->colorBuffer[i] & 7), 40);
                lastCol = g_renderer->colorBuffer[i];
            }
            putchar(g_renderer->backBuffer[i]);
        }
        if(y < g_renderer->height - 1) putchar('\n');
    }
#endif
    fflush(stdout);
    char* tmp = g_renderer->frontBuffer;
    g_renderer->frontBuffer = g_renderer->backBuffer;
    g_renderer->backBuffer = tmp;
}

void maintain_frame_rate() {
    clock_t now = clock();
    float elapsed = ((float)(now - g_renderer->lastFrame)) / CLOCKS_PER_SEC * 1000.0f;
    float targetTime = 1000.0f / g_renderer->targetFPS;
    
    if(elapsed < targetTime) {
        int sleepMs = (int)(targetTime - elapsed);
        SLEEP(sleepMs);
    }
    
    g_renderer->lastFrame = clock();
}

int load_texture(const char* name) {
    int texId = g_renderer->textureCount++;
    g_renderer->textures = (unsigned int**)realloc(g_renderer->textures, g_renderer->textureCount * sizeof(unsigned int*));
    g_renderer->textureWidths = (int*)realloc(g_renderer->textureWidths, g_renderer->textureCount * sizeof(int));
    g_renderer->textureHeights = (int*)realloc(g_renderer->textureHeights, g_renderer->textureCount * sizeof(int));
    
    // Generate procedural textures
    if(strcmp(name, "grass") == 0) {
        g_renderer->textureWidths[texId] = 8;
        g_renderer->textureHeights[texId] = 8;
        g_renderer->textures[texId] = (unsigned int*)malloc(64 * sizeof(unsigned int));
        for(int i = 0; i < 64; i++) {
            int noise = (i * 7) % 3;
            g_renderer->textures[texId][i] = 0x228B22 + (noise * 0x001100);
        }
    } else if(strcmp(name, "stone") == 0) {
        g_renderer->textureWidths[texId] = 8;
        g_renderer->textureHeights[texId] = 8;
        g_renderer->textures[texId] = (unsigned int*)malloc(64 * sizeof(unsigned int));
        for(int i = 0; i < 64; i++) {
            int noise = (i * 3) % 4;
            g_renderer->textures[texId][i] = 0x696969 + (noise * 0x111111);
        }
    } else {
        // Default white texture
        g_renderer->textureWidths[texId] = 1;
        g_renderer->textureHeights[texId] = 1;
        g_renderer->textures[texId] = (unsigned int*)malloc(sizeof(unsigned int));
        g_renderer->textures[texId][0] = 0xFFFFFF;
    }
    
    return texId;
}

void enhanced_cleanup() {
#ifdef _WIN32
    SetConsoleMode(g_renderer->hConsoleIn, g_renderer->oldMode);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(g_renderer->hConsole, &ci);
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(g_renderer->hConsole, &ci);
    SetConsoleTextAttribute(g_renderer->hConsole, 7);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &g_renderer->oldTermios);
    printf("\033[?25h\033[0m\n");
#endif
    
    if(g_renderer) {
        free(g_renderer->frontBuffer);
        free(g_renderer->backBuffer);
        free(g_renderer->depthBuffer);
        free(g_renderer->colorBuffer);
        for(int i = 0; i < g_renderer->textureCount; i++) {
            free(g_renderer->textures[i]);
        }
        free(g_renderer->textures);
        free(g_renderer->textureWidths);
        free(g_renderer->textureHeights);
        free(g_renderer);
    }
}
)";
    }
    
    std::string emitSoundSystem() {
        return R"(
/* Sound System for .dish and .disll files */
typedef struct {
    short* samples;
    int sampleCount;
    int sampleRate;
    int channels;
    int looping;
} SoundClip;

static SoundClip* loadedSounds = NULL;
static char** soundNames = NULL;
static int soundCount = 0;

#ifdef _WIN32
static HWAVEOUT hWaveOut;
#endif

void sound_system_init() {
#ifdef _WIN32
    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
#endif
}

int load_sound_dish(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if(!file) {
        // Create dummy sound for demo
        int soundId = soundCount++;
        loadedSounds = (SoundClip*)realloc(loadedSounds, soundCount * sizeof(SoundClip));
        soundNames = (char**)realloc(soundNames, soundCount * sizeof(char*));
        
        soundNames[soundId] = (char*)malloc(strlen(filename) + 1);
        strcpy(soundNames[soundId], filename);
        
        loadedSounds[soundId].sampleCount = 4410; // 0.1 second at 44.1kHz
        loadedSounds[soundId].samples = (short*)malloc(4410 * sizeof(short));
        loadedSounds[soundId].sampleRate = 44100;
        loadedSounds[soundId].channels = 1;
        loadedSounds[soundId].looping = 0;
        
        // Generate simple beep
        for(int i = 0; i < 4410; i++) {
            loadedSounds[soundId].samples[i] = (short)(sin(i * 0.1) * 16000);
        }
        
        return soundId;
    }
    
    // TODO: Implement actual DISH file loading
    fclose(file);
    return 0;
}

int load_sound_disll(const char* filename) {
    int soundId = load_sound_dish(filename);
    if(soundId >= 0) {
        loadedSounds[soundId].looping = 1;
    }
    return soundId;
}

void play_sound(const char* filename) {
    for(int i = 0; i < soundCount; i++) {
        if(strcmp(soundNames[i], filename) == 0) {
#ifdef _WIN32
            WAVEHDR hdr = {0};
            hdr.lpData = (LPSTR)loadedSounds[i].samples;
            hdr.dwBufferLength = loadedSounds[i].sampleCount * sizeof(short);
            waveOutPrepareHeader(hWaveOut, &hdr, sizeof(WAVEHDR));
            waveOutWrite(hWaveOut, &hdr, sizeof(WAVEHDR));
#endif
            break;
        }
    }
}

void sound_system_cleanup() {
#ifdef _WIN32
    waveOutClose(hWaveOut);
#endif
    for(int i = 0; i < soundCount; i++) {
        free(loadedSounds[i].samples);
        free(soundNames[i]);
    }
    free(loadedSounds);
    free(soundNames);
}
)";
    }
    
    std::string emitVoxelWorld() {
        return R"(
/* Voxel World Generation */
#define CHUNK_SIZE 16
#define WORLD_HEIGHT 64

typedef struct {
    unsigned char type; // 0=air, 1=grass, 2=dirt, 3=stone, 4=wood
} Voxel;

static Voxel* chunks[256]; // Simple chunk storage
static int chunkKeys[256];
static int chunkCount = 0;

int getChunkKey(int chunkX, int chunkZ) {
    return (chunkX & 0xFF) | ((chunkZ & 0xFF) << 8);
}

Voxel* getOrCreateChunk(int chunkX, int chunkZ) {
    int key = getChunkKey(chunkX, chunkZ);
    
    // Find existing chunk
    for(int i = 0; i < chunkCount; i++) {
        if(chunkKeys[i] == key) {
            return chunks[i];
        }
    }
    
    // Create new chunk
    if(chunkCount >= 256) return NULL; // Chunk limit
    
    Voxel* chunk = (Voxel*)malloc(CHUNK_SIZE * WORLD_HEIGHT * CHUNK_SIZE * sizeof(Voxel));
    
    // Generate terrain
    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int z = 0; z < CHUNK_SIZE; z++) {
            int worldX = chunkX * CHUNK_SIZE + x;
            int worldZ = chunkZ * CHUNK_SIZE + z;
            
            // Simple terrain generation
            int height = 32 + (int)(8 * sin(worldX * 0.1f) * cos(worldZ * 0.1f));
            
            for(int y = 0; y < WORLD_HEIGHT; y++) {
                int idx = y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
                
                if(y < height - 3) chunk[idx].type = 3; // Stone
                else if(y < height - 1) chunk[idx].type = 2; // Dirt
                else if(y < height) chunk[idx].type = 1; // Grass
                else chunk[idx].type = 0; // Air
            }
        }
    }
    
    chunks[chunkCount] = chunk;
    chunkKeys[chunkCount] = key;
    chunkCount++;
    
    return chunk;
}

Mesh generate_voxel_chunk(int chunkX, int chunkZ) {
    Mesh mesh;
    mesh.count = 0;
    mesh.tris = NULL;
    
    Voxel* chunk = getOrCreateChunk(chunkX, chunkZ);
    if(!chunk) return mesh;
    
    // Count triangles first
    int triCount = 0;
    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < WORLD_HEIGHT; y++) {
            for(int z = 0; z < CHUNK_SIZE; z++) {
                int idx = y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
                if(chunk[idx].type == 0) continue;
                
                // Check each face
                if(x == 0 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + (x-1)].type == 0) triCount += 2;
                if(x == CHUNK_SIZE-1 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + (x+1)].type == 0) triCount += 2;
                if(y == 0 || chunk[(y-1) * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x].type == 0) triCount += 2;
                if(y == WORLD_HEIGHT-1 || chunk[(y+1) * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x].type == 0) triCount += 2;
                if(z == 0 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + (z-1) * CHUNK_SIZE + x].type == 0) triCount += 2;
                if(z == CHUNK_SIZE-1 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + (z+1) * CHUNK_SIZE + x].type == 0) triCount += 2;
            }
        }
    }
    
    mesh.tris = (Triangle*)malloc(triCount * sizeof(Triangle));
    mesh.count = 0;
    
    // Generate triangles
    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < WORLD_HEIGHT; y++) {
            for(int z = 0; z < CHUNK_SIZE; z++) {
                int idx = y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
                if(chunk[idx].type == 0) continue;
                
                float fx = (float)x + chunkX * CHUNK_SIZE;
                float fy = (float)y;
                float fz = (float)z + chunkZ * CHUNK_SIZE;
                int color = chunk[idx].type + 1;
                
                // Add cube faces (simplified - just front and top for performance)
                if(z == CHUNK_SIZE-1 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + (z+1) * CHUNK_SIZE + x].type == 0) {
                    mesh.tris[mesh.count++] = (Triangle){{{fx,fy,fz+1},{fx,fy+1,fz+1},{fx+1,fy+1,fz+1}}, color};
                    mesh.tris[mesh.count++] = (Triangle){{{fx,fy,fz+1},{fx+1,fy+1,fz+1},{fx+1,fy,fz+1}}, color};
                }
                
                if(y == WORLD_HEIGHT-1 || chunk[(y+1) * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x].type == 0) {
                    mesh.tris[mesh.count++] = (Triangle){{{fx,fy+1,fz},{fx,fy+1,fz+1},{fx+1,fy+1,fz+1}}, color};
                    mesh.tris[mesh.count++] = (Triangle){{{fx,fy+1,fz},{fx+1,fy+1,fz+1},{fx+1,fy+1,fz}}, color};
                }
            }
        }
    }
    
    return mesh;
}
)";
    }
    
    std::string emitRaytracing() {
        return R"(
/* Simple Raytracing */
typedef struct {
    Vec3 origin, direction;
} Ray;

typedef struct {
    int hit;
    float distance;
    Vec3 point, normal;
    int materialId;
} RayHit;

RayHit raycast_triangle(Ray ray, Triangle tri) {
    RayHit result = {0, 1000.0f, {0,0,0}, {0,0,0}, 0};
    
    Vec3 edge1 = vec3_sub(tri.v[1], tri.v[0]);
    Vec3 edge2 = vec3_sub(tri.v[2], tri.v[0]);
    Vec3 h = vec3_cross(ray.direction, edge2);
    float a = vec3_dot(edge1, h);
    
    if(a > -0.00001f && a < 0.00001f) return result;
    
    float f = 1.0f / a;
    Vec3 s = vec3_sub(ray.origin, tri.v[0]);
    float u = f * vec3_dot(s, h);
    
    if(u < 0.0f || u > 1.0f) return result;
    
    Vec3 q = vec3_cross(s, edge1);
    float v = f * vec3_dot(ray.direction, q);
    
    if(v < 0.0f || u + v > 1.0f) return result;
    
    float t = f * vec3_dot(edge2, q);
    
    if(t > 0.00001f) {
        result.hit = 1;
        result.distance = t;
        result.point = vec3_add(ray.origin, vec3_mul(ray.direction, t));
        result.normal = vec3_norm(vec3_cross(edge1, edge2));
        result.materialId = tri.color;
    }
    
    return result;
}

void enhanced_render_raytraced(Mesh mesh, Mat4 transform, Mat4 proj) {
    Vec3 lightPos = {2.0f, 3.0f, -1.0f};
    
    for(int y = 0; y < g_renderer->height; y += 2) {
        for(int x = 0; x < g_renderer->width; x += 2) {
            float screenX = (2.0f * x / g_renderer->width) - 1.0f;
            float screenY = 1.0f - (2.0f * y / g_renderer->height);
            
            Vec3 rayDir = {screenX, screenY, 1.0f};
            rayDir = vec3_norm(rayDir);
            Ray ray = {{0, 0, -5}, rayDir};
            
            RayHit closest = {0, 1000.0f, {0,0,0}, {0,0,0}, 0};
            
            for(int i = 0; i < mesh.count; i++) {
                Triangle worldTri;
                worldTri.v[0] = mat4_mulv(&transform, mesh.tris[i].v[0]);
                worldTri.v[1] = mat4_mulv(&transform, mesh.tris[i].v[1]);
                worldTri.v[2] = mat4_mulv(&transform, mesh.tris[i].v[2]);
                worldTri.color = mesh.tris[i].color;
                
                RayHit hit = raycast_triangle(ray, worldTri);
                if(hit.hit && hit.distance < closest.distance) {
                    closest = hit;
                }
            }
            
            if(closest.hit) {
                Vec3 lightDir = vec3_norm(vec3_sub(lightPos, closest.point));
                float dp = fmaxf(0.1f, vec3_dot(closest.normal, lightDir));
                
                int shadeIdx = (int)(dp * (strlen(advancedShade) - 1));
                if(shadeIdx >= strlen(advancedShade)) shadeIdx = strlen(advancedShade) - 1;
                char shadeChar = advancedShade[shadeIdx];
                
                enhanced_setpixel(x, y, shadeChar, closest.distance, closest.materialId);
                if(x + 1 < g_renderer->width) enhanced_setpixel(x + 1, y, shadeChar, closest.distance, closest.materialId);
                if(y + 1 < g_renderer->height) enhanced_setpixel(x, y + 1, shadeChar, closest.distance, closest.materialId);
                if(x + 1 < g_renderer->width && y + 1 < g_renderer->height) enhanced_setpixel(x + 1, y + 1, shadeChar, closest.distance, closest.materialId);
            }
        }
    }
}

static int raytracingEnabled = 0;
void toggle_raytracing() { raytracingEnabled = !raytracingEnabled; }
)";
    }
    
    std::string emitEnhancedRuntimeFunctions() {
        return R"(
/* Enhanced Runtime Functions */
int get_key() {
#ifdef _WIN32
    if(_kbhit()) {
        int ch = _getch();
        if(ch == 0 || ch == 224) {
            ch = _getch();
            switch(ch) {
                case 72: return 256; // UP
                case 80: return 257; // DOWN
                case 75: return 258; // LEFT
                case 77: return 259; // RIGHT
            }
            return ch + 256;
        }
        return ch;
    }
    return 0;
#else
    int ch = getchar();
    if(ch == EOF || ch == -1) return 0;
    if(ch == 27) {
        int c2 = getchar();
        if(c2 == '[') {
            int c3 = getchar();
            switch(c3) {
                case 'A': return 256; // UP
                case 'B': return 257; // DOWN
                case 'D': return 258; // LEFT
                case 'C': return 259; // RIGHT
            }
        }
        return 27;
    }
    return ch;
#endif
}

long get_time_ms() {
    return clock() * 1000 / CLOCKS_PER_SEC;
}

void enhanced_draw_text(int x, int y, const char* text, int color) {
    int len = strlen(text);
    for(int i = 0; i < len; i++) {
        enhanced_setpixel(x + i, y, text[i], 1.0f, color);
    }
}

void enhanced_draw_box(int x, int y, int w, int h, int color) {
    for(int i = x; i < x + w; i++) {
        enhanced_setpixel(i, y, '-', 1.0f, color);
        enhanced_setpixel(i, y + h - 1, '-', 1.0f, color);
    }
    for(int i = y; i < y + h; i++) {
        enhanced_setpixel(x, i, '|', 1.0f, color);
        enhanced_setpixel(x + w - 1, i, '|', 1.0f, color);
    }
    enhanced_setpixel(x, y, '+', 1.0f, color);
    enhanced_setpixel(x + w - 1, y, '+', 1.0f, color);
    enhanced_setpixel(x, y + h - 1, '+', 1.0f, color);
    enhanced_setpixel(x + w - 1, y + h - 1, '+', 1.0f, color);
}

Mat4 mat4_projection(float fov, float aspect, float znear, float zfar) {
    return mat4_proj(fov, aspect, znear, zfar);
}

static Vec3 cameraPos = {0, 0, 0};
static float cameraYaw = 0, cameraPitch = 0;

void set_camera_pos(float x, float y, float z) {
    cameraPos.x = x; cameraPos.y = y; cameraPos.z = z;
}

void set_camera_rotation(float yaw, float pitch) {
    cameraYaw = yaw; cameraPitch = pitch;
}

void enhanced_render_textured(Mesh mesh, Mat4 transform, Mat4 proj, int textureId) {
    Vec3 lightDir = {0.3f, -0.7f, -0.6f};
    lightDir = vec3_norm(lightDir);
    
    for(int i = 0; i < mesh.count; i++) {
        Triangle tri = mesh.tris[i];
        Vec3 v0 = mat4_mulv(&transform, tri.v[0]);
        Vec3 v1 = mat4_mulv(&transform, tri.v[1]);
        Vec3 v2 = mat4_mulv(&transform, tri.v[2]);
        
        Vec3 line1 = vec3_sub(v1, v0);
        Vec3 line2 = vec3_sub(v2, v0);
        Vec3 normal = vec3_norm(vec3_cross(line1, line2));
        
        if(vec3_dot(normal, v0) >= 0) continue;
        
        float dp = fmaxf(0.1f, vec3_dot(normal, lightDir));
        
        Vec3 p0 = mat4_mulv(&proj, v0);
        Vec3 p1 = mat4_mulv(&proj, v1);
        Vec3 p2 = mat4_mulv(&proj, v2);
        
        int sx0 = (int)((p0.x + 1) * 0.5f * g_renderer->width);
        int sy0 = (int)((p0.y + 1) * 0.5f * g_renderer->height);
        int sx1 = (int)((p1.x + 1) * 0.5f * g_renderer->width);
        int sy1 = (int)((p1.y + 1) * 0.5f * g_renderer->height);
        int sx2 = (int)((p2.x + 1) * 0.5f * g_renderer->width);
        int sy2 = (int)((p2.y + 1) * 0.5f * g_renderer->height);
        
        float avgZ = (p0.z + p1.z + p2.z) / 3.0f;
        
        int shadeIdx = (int)(dp * (strlen(advancedShade) - 1));
        if(shadeIdx >= strlen(advancedShade)) shadeIdx = strlen(advancedShade) - 1;
        char shadeChar = advancedShade[shadeIdx];
        
        // Simple triangle fill (placeholder)
        enhanced_setpixel(sx0, sy0, shadeChar, avgZ, tri.color);
        enhanced_setpixel(sx1, sy1, shadeChar, avgZ, tri.color);
        enhanced_setpixel(sx2, sy2, shadeChar, avgZ, tri.color);
    }
}
)";
    }
    
    std::string generateEnhancedStatement(const StmtPtr& stmt) {
        // Placeholder - would need full statement generation
        return "/* Enhanced statement */";
    }
};

} // namespace disp

#endif // DISPLEXITY_ENHANCED_TUIGEN_H