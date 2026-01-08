#ifndef DISPLEXITY_ENHANCED_TUI_RENDERER_H
#define DISPLEXITY_ENHANCED_TUI_RENDERER_H

#include "renderer3d.h"
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#else
#include <pulse/simple.h>
#include <pulse/error.h>
#endif

namespace disp {

// Enhanced texture system
struct Texture {
    int width, height;
    std::vector<uint32_t> pixels;
    
    Texture(int w = 1, int h = 1) : width(w), height(h) {
        pixels.resize(w * h, 0xFFFFFF);
    }
    
    uint32_t sample(float u, float v) const {
        int x = (int)(u * width) % width;
        int y = (int)(v * height) % height;
        if (x < 0) x += width;
        if (y < 0) y += height;
        return pixels[y * width + x];
    }
};

// Sound system for .dish and .disll files
class SoundSystem {
private:
    struct SoundClip {
        std::vector<int16_t> samples;
        int sampleRate;
        int channels;
        bool looping;
    };
    
    std::map<std::string, SoundClip> loadedSounds;
    
#ifdef _WIN32
    HWAVEOUT hWaveOut;
    std::vector<WAVEHDR> waveHeaders;
#else
    pa_simple* pulseAudio;
#endif

public:
    SoundSystem() {
#ifdef _WIN32
        WAVEFORMATEX wfx = {0};
        wfx.wFormatTag = WAVE_FORMAT_PCM;
        wfx.nChannels = 2;
        wfx.nSamplesPerSec = 44100;
        wfx.wBitsPerSample = 16;
        wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
        wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
        waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
#else
        pa_sample_spec ss = {PA_SAMPLE_S16LE, 44100, 2};
        pulseAudio = pa_simple_new(NULL, "Displexity", PA_STREAM_PLAYBACK, NULL, "game", &ss, NULL, NULL, NULL);
#endif
    }
    
    ~SoundSystem() {
#ifdef _WIN32
        waveOutClose(hWaveOut);
#else
        if (pulseAudio) pa_simple_free(pulseAudio);
#endif
    }
    
    // Load .dish (Displexity Sound Header) files
    bool loadDish(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return false;
        
        // Read DISH header
        char magic[8];
        file.read(magic, 8);
        if (strncmp(magic, "DISHSND1", 8) != 0) return false;
        
        uint32_t sampleRate, channels, sampleCount;
        file.read((char*)&sampleRate, 4);
        file.read((char*)&channels, 4);
        file.read((char*)&sampleCount, 4);
        
        SoundClip clip;
        clip.sampleRate = sampleRate;
        clip.channels = channels;
        clip.samples.resize(sampleCount);
        clip.looping = false;
        
        file.read((char*)clip.samples.data(), sampleCount * sizeof(int16_t));
        loadedSounds[filename] = clip;
        return true;
    }
    
    // Load .disll (Displexity Sound Loop Library) files
    bool loadDisll(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return false;
        
        // Read DISLL header
        char magic[8];
        file.read(magic, 8);
        if (strncmp(magic, "DISLLP1", 8) != 0) return false;
        
        uint32_t sampleRate, channels, sampleCount;
        file.read((char*)&sampleRate, 4);
        file.read((char*)&channels, 4);
        file.read((char*)&sampleCount, 4);
        
        SoundClip clip;
        clip.sampleRate = sampleRate;
        clip.channels = channels;
        clip.samples.resize(sampleCount);
        clip.looping = true;  // DISLL files are for looping
        
        file.read((char*)clip.samples.data(), sampleCount * sizeof(int16_t));
        loadedSounds[filename] = clip;
        return true;
    }
    
    void playSound(const std::string& filename) {
        auto it = loadedSounds.find(filename);
        if (it == loadedSounds.end()) return;
        
        const SoundClip& clip = it->second;
        
#ifdef _WIN32
        WAVEHDR hdr = {0};
        hdr.lpData = (LPSTR)clip.samples.data();
        hdr.dwBufferLength = clip.samples.size() * sizeof(int16_t);
        waveOutPrepareHeader(hWaveOut, &hdr, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &hdr, sizeof(WAVEHDR));
#else
        if (pulseAudio) {
            pa_simple_write(pulseAudio, clip.samples.data(), clip.samples.size() * sizeof(int16_t), NULL);
        }
#endif
    }
};

// Enhanced TUI Renderer with 200 FPS target and advanced features
class EnhancedTuiRenderer : public TuiRenderer {
private:
    std::vector<Texture> textures;
    std::unique_ptr<SoundSystem> soundSystem;
    std::chrono::high_resolution_clock::time_point lastFrame;
    float targetFPS = 200.0f;
    float frameTime = 1000.0f / targetFPS; // 5ms per frame
    
    // Raytracing data
    struct Ray {
        Vec3 origin, direction;
        Ray(Vec3 o, Vec3 d) : origin(o), direction(d) {}
    };
    
    struct RayHit {
        bool hit;
        float distance;
        Vec3 point, normal;
        int materialId;
        RayHit() : hit(false), distance(1000.0f), materialId(0) {}
    };
    
    // Advanced shading characters for better quality
    const char* advancedShade = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    
public:
    EnhancedTuiRenderer(int w = 120, int h = 40) : TuiRenderer(w, h) {
        soundSystem = std::make_unique<SoundSystem>();
        lastFrame = std::chrono::high_resolution_clock::now();
        
        // Create default white texture
        textures.emplace_back(1, 1);
    }
    
    // Load texture from simple format (for now, create procedural textures)
    int loadTexture(const std::string& filename) {
        if (filename == "brick") {
            Texture tex(16, 16);
            for (int y = 0; y < 16; y++) {
                for (int x = 0; x < 16; x++) {
                    bool isBrick = ((x / 4) + (y / 2)) % 2 == 0;
                    tex.pixels[y * 16 + x] = isBrick ? 0x8B4513 : 0x654321;
                }
            }
            textures.push_back(tex);
            return textures.size() - 1;
        } else if (filename == "grass") {
            Texture tex(8, 8);
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    int noise = (x * 7 + y * 13) % 3;
                    uint32_t green = 0x228B22 + (noise * 0x001100);
                    tex.pixels[y * 8 + x] = green;
                }
            }
            textures.push_back(tex);
            return textures.size() - 1;
        } else if (filename == "stone") {
            Texture tex(8, 8);
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    int noise = (x * 3 + y * 5) % 4;
                    uint32_t gray = 0x696969 + (noise * 0x111111);
                    tex.pixels[y * 8 + x] = gray;
                }
            }
            textures.push_back(tex);
            return textures.size() - 1;
        }
        return 0; // Default white texture
    }
    
    // Enhanced render with texture mapping and better lighting
    void renderTextured(const Mesh& mesh, const Mat4& transform, const Mat4& projection, int textureId = 0) {
        Vec3 lightDir(0.3f, -0.7f, -0.6f);
        lightDir = lightDir.normalize();
        
        const Texture& tex = (textureId < textures.size()) ? textures[textureId] : textures[0];
        
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
            
            // Sample texture at triangle center for color
            uint32_t texColor = tex.sample(0.5f, 0.5f);
            float r = ((texColor >> 16) & 0xFF) / 255.0f;
            float g = ((texColor >> 8) & 0xFF) / 255.0f;
            float b = (texColor & 0xFF) / 255.0f;
            
            // Apply lighting
            float brightness = dp;
            int shadeIdx = (int)(brightness * (strlen(advancedShade) - 1));
            if (shadeIdx >= strlen(advancedShade)) shadeIdx = strlen(advancedShade) - 1;
            char shadeChar = advancedShade[shadeIdx];
            
            // Convert to console color (approximate)
            int color = 7; // Default white
            if (r > g && r > b) color = 12; // Red
            else if (g > r && g > b) color = 10; // Green
            else if (b > r && b > g) color = 9;  // Blue
            else if (r > 0.7f && g > 0.7f) color = 14; // Yellow
            else if (r > 0.5f && b > 0.5f) color = 13; // Magenta
            else if (g > 0.5f && b > 0.5f) color = 11; // Cyan
            else if (r + g + b < 1.5f) color = 8; // Dark gray
            
            fillTriangle(sx0, sy0, avgZ, sx1, sy1, avgZ, sx2, sy2, avgZ, shadeChar, color);
        }
    }
    
    // Simple raytracing for reflections and shadows
    RayHit raycast(const Ray& ray, const std::vector<Triangle>& scene) {
        RayHit closest;
        
        for (const auto& tri : scene) {
            // Ray-triangle intersection using Möller-Trumbore algorithm
            Vec3 edge1 = tri.v[1] - tri.v[0];
            Vec3 edge2 = tri.v[2] - tri.v[0];
            Vec3 h = ray.direction.cross(edge2);
            float a = edge1.dot(h);
            
            if (a > -0.00001f && a < 0.00001f) continue;
            
            float f = 1.0f / a;
            Vec3 s = ray.origin - tri.v[0];
            float u = f * s.dot(h);
            
            if (u < 0.0f || u > 1.0f) continue;
            
            Vec3 q = s.cross(edge1);
            float v = f * ray.direction.dot(q);
            
            if (v < 0.0f || u + v > 1.0f) continue;
            
            float t = f * edge2.dot(q);
            
            if (t > 0.00001f && t < closest.distance) {
                closest.hit = true;
                closest.distance = t;
                closest.point = ray.origin + ray.direction * t;
                closest.normal = edge1.cross(edge2).normalize();
                closest.materialId = tri.color;
            }
        }
        
        return closest;
    }
    
    // Render with basic raytracing effects
    void renderRaytraced(const Mesh& mesh, const Mat4& transform, const Mat4& projection) {
        std::vector<Triangle> worldTris;
        
        // Transform triangles to world space
        for (const auto& tri : mesh.tris) {
            Triangle worldTri;
            worldTri.v[0] = transform.multiply(tri.v[0]);
            worldTri.v[1] = transform.multiply(tri.v[1]);
            worldTri.v[2] = transform.multiply(tri.v[2]);
            worldTri.color = tri.color;
            worldTris.push_back(worldTri);
        }
        
        Vec3 lightPos(2.0f, 3.0f, -1.0f);
        
        // Render each pixel with raytracing
        for (int y = 0; y < height; y += 2) { // Skip every other row for performance
            for (int x = 0; x < width; x += 2) { // Skip every other column
                // Convert screen coordinates to world ray
                float screenX = (2.0f * x / width) - 1.0f;
                float screenY = 1.0f - (2.0f * y / height);
                
                Vec3 rayDir(screenX, screenY, 1.0f);
                rayDir = rayDir.normalize();
                Ray ray(Vec3(0, 0, -5), rayDir);
                
                RayHit hit = raycast(ray, worldTris);
                
                if (hit.hit) {
                    // Calculate lighting
                    Vec3 lightDir = (lightPos - hit.point).normalize();
                    float dp = std::max(0.1f, hit.normal.dot(lightDir));
                    
                    // Check for shadows
                    Ray shadowRay(hit.point + hit.normal * 0.001f, lightDir);
                    RayHit shadowHit = raycast(shadowRay, worldTris);
                    if (shadowHit.hit && shadowHit.distance < (lightPos - hit.point).length()) {
                        dp *= 0.3f; // In shadow
                    }
                    
                    int shadeIdx = (int)(dp * (strlen(advancedShade) - 1));
                    if (shadeIdx >= strlen(advancedShade)) shadeIdx = strlen(advancedShade) - 1;
                    char shadeChar = advancedShade[shadeIdx];
                    
                    setPixel(x, y, shadeChar, hit.distance, hit.materialId);
                    if (x + 1 < width) setPixel(x + 1, y, shadeChar, hit.distance, hit.materialId);
                    if (y + 1 < height) setPixel(x, y + 1, shadeChar, hit.distance, hit.materialId);
                    if (x + 1 < width && y + 1 < height) setPixel(x + 1, y + 1, shadeChar, hit.distance, hit.materialId);
                }
            }
        }
    }
    
    // Frame rate control for 200 FPS
    void maintainFrameRate() {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastFrame).count();
        float targetMicros = frameTime * 1000.0f;
        
        if (elapsed < targetMicros) {
            int sleepMicros = (int)(targetMicros - elapsed);
            std::this_thread::sleep_for(std::chrono::microseconds(sleepMicros));
        }
        
        lastFrame = std::chrono::high_resolution_clock::now();
    }
    
    // Sound system integration
    void playSound(const std::string& filename) {
        soundSystem->playSound(filename);
    }
    
    bool loadSoundDish(const std::string& filename) {
        return soundSystem->loadDish(filename);
    }
    
    bool loadSoundDisll(const std::string& filename) {
        return soundSystem->loadDisll(filename);
    }
    
    // Enhanced window resizing support
    void resize(int newWidth, int newHeight) {
        if (newWidth == width && newHeight == height) return;
        
        width = newWidth;
        height = newHeight;
        
        int newSize = width * height;
        frontBuffer.resize(newSize, ' ');
        backBuffer.resize(newSize, ' ');
        depthBuffer.resize(newSize, 0.0f);
        colorBuffer.resize(newSize, 7);
        
#ifdef _WIN32
        // Update console buffer size
        COORD newSize_coord = {(SHORT)width, (SHORT)height};
        SetConsoleScreenBufferSize(hConsole, newSize_coord);
#endif
    }
    
    // Get current FPS
    float getCurrentFPS() const {
        return 1000.0f / frameTime;
    }
    
    void setTargetFPS(float fps) {
        targetFPS = fps;
        frameTime = 1000.0f / fps;
    }
};

// Minecraft-like voxel world for demonstration
class VoxelWorld {
private:
    static const int CHUNK_SIZE = 16;
    static const int WORLD_HEIGHT = 64;
    
    struct Voxel {
        uint8_t type; // 0=air, 1=grass, 2=dirt, 3=stone, 4=wood
        Voxel(uint8_t t = 0) : type(t) {}
    };
    
    std::map<std::pair<int, int>, std::vector<Voxel>> chunks;
    
public:
    Mesh generateChunkMesh(int chunkX, int chunkZ) {
        Mesh mesh;
        auto key = std::make_pair(chunkX, chunkZ);
        
        if (chunks.find(key) == chunks.end()) {
            // Generate new chunk
            std::vector<Voxel> chunk(CHUNK_SIZE * WORLD_HEIGHT * CHUNK_SIZE);
            
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    int worldX = chunkX * CHUNK_SIZE + x;
                    int worldZ = chunkZ * CHUNK_SIZE + z;
                    
                    // Simple terrain generation
                    int height = 32 + (int)(8 * sin(worldX * 0.1f) * cos(worldZ * 0.1f));
                    
                    for (int y = 0; y < WORLD_HEIGHT; y++) {
                        int idx = y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
                        
                        if (y < height - 3) chunk[idx] = Voxel(3); // Stone
                        else if (y < height - 1) chunk[idx] = Voxel(2); // Dirt
                        else if (y < height) chunk[idx] = Voxel(1); // Grass
                        else chunk[idx] = Voxel(0); // Air
                    }
                }
            }
            
            chunks[key] = chunk;
        }
        
        // Generate mesh from voxels
        const auto& chunk = chunks[key];
        
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < WORLD_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    int idx = y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
                    
                    if (chunk[idx].type == 0) continue; // Skip air
                    
                    float fx = (float)x + chunkX * CHUNK_SIZE;
                    float fy = (float)y;
                    float fz = (float)z + chunkZ * CHUNK_SIZE;
                    
                    int color = chunk[idx].type + 1; // Map voxel type to color
                    
                    // Add cube faces (only if adjacent voxel is air)
                    // Front face
                    if (z == CHUNK_SIZE - 1 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + (z + 1) * CHUNK_SIZE + x].type == 0) {
                        mesh.tris.push_back(Triangle(Vec3(fx, fy, fz + 1), Vec3(fx, fy + 1, fz + 1), Vec3(fx + 1, fy + 1, fz + 1), color));
                        mesh.tris.push_back(Triangle(Vec3(fx, fy, fz + 1), Vec3(fx + 1, fy + 1, fz + 1), Vec3(fx + 1, fy, fz + 1), color));
                    }
                    
                    // Back face
                    if (z == 0 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + (z - 1) * CHUNK_SIZE + x].type == 0) {
                        mesh.tris.push_back(Triangle(Vec3(fx + 1, fy, fz), Vec3(fx + 1, fy + 1, fz), Vec3(fx, fy + 1, fz), color));
                        mesh.tris.push_back(Triangle(Vec3(fx + 1, fy, fz), Vec3(fx, fy + 1, fz), Vec3(fx, fy, fz), color));
                    }
                    
                    // Top face
                    if (y == WORLD_HEIGHT - 1 || chunk[(y + 1) * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x].type == 0) {
                        mesh.tris.push_back(Triangle(Vec3(fx, fy + 1, fz), Vec3(fx, fy + 1, fz + 1), Vec3(fx + 1, fy + 1, fz + 1), color));
                        mesh.tris.push_back(Triangle(Vec3(fx, fy + 1, fz), Vec3(fx + 1, fy + 1, fz + 1), Vec3(fx + 1, fy + 1, fz), color));
                    }
                    
                    // Bottom face
                    if (y == 0 || chunk[(y - 1) * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x].type == 0) {
                        mesh.tris.push_back(Triangle(Vec3(fx + 1, fy, fz), Vec3(fx + 1, fy, fz + 1), Vec3(fx, fy, fz + 1), color));
                        mesh.tris.push_back(Triangle(Vec3(fx + 1, fy, fz), Vec3(fx, fy, fz + 1), Vec3(fx, fy, fz), color));
                    }
                    
                    // Right face
                    if (x == CHUNK_SIZE - 1 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + (x + 1)].type == 0) {
                        mesh.tris.push_back(Triangle(Vec3(fx + 1, fy, fz), Vec3(fx + 1, fy + 1, fz), Vec3(fx + 1, fy + 1, fz + 1), color));
                        mesh.tris.push_back(Triangle(Vec3(fx + 1, fy, fz), Vec3(fx + 1, fy + 1, fz + 1), Vec3(fx + 1, fy, fz + 1), color));
                    }
                    
                    // Left face
                    if (x == 0 || chunk[y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + (x - 1)].type == 0) {
                        mesh.tris.push_back(Triangle(Vec3(fx, fy, fz + 1), Vec3(fx, fy + 1, fz + 1), Vec3(fx, fy + 1, fz), color));
                        mesh.tris.push_back(Triangle(Vec3(fx, fy, fz + 1), Vec3(fx, fy + 1, fz), Vec3(fx, fy, fz), color));
                    }
                }
            }
        }
        
        return mesh;
    }
};

} // namespace disp

#endif // DISPLEXITY_ENHANCED_TUI_RENDERER_H