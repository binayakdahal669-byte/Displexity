// Displexity Graphics and Input API
// Cross-platform abstraction layer with framebuffer support

#ifndef DISPLEXITY_GRAPHICS_H
#define DISPLEXITY_GRAPHICS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === FRAMEBUFFER STRUCTURE ===
typedef struct {
    uint32_t* pixels;        // RGBA pixel data
    uint32_t width;
    uint32_t height;
    uint32_t pitch;          // bytes per scanline
    uint8_t* backbuffer;     // double-buffering
    int use_backbuffer;
} dis_framebuffer;

// Graphics context
typedef struct {
    uint32_t width;
    uint32_t height;
    dis_framebuffer fb;
    void* platform_handle;   // HWND on Windows, Display* on Linux
} dis_graphics_ctx;

// Color (RGBA)
typedef uint32_t dis_color;

#define DIS_COLOR(r, g, b, a) ((a << 24) | (b << 16) | (g << 8) | r)
#define DIS_RED   DIS_COLOR(255, 0, 0, 255)
#define DIS_GREEN DIS_COLOR(0, 255, 0, 255)
#define DIS_BLUE  DIS_COLOR(0, 0, 255, 255)
#define DIS_WHITE DIS_COLOR(255, 255, 255, 255)
#define DIS_BLACK DIS_COLOR(0, 0, 0, 255)
#define DIS_CYAN  DIS_COLOR(0, 255, 255, 255)
#define DIS_MAGENTA DIS_COLOR(255, 0, 255, 255)
#define DIS_YELLOW DIS_COLOR(255, 255, 0, 255)

// Input event types
enum dis_key_code {
    DIS_KEY_UNKNOWN = 0,
    DIS_KEY_A = 1, DIS_KEY_B, DIS_KEY_C, DIS_KEY_D, DIS_KEY_E,
    DIS_KEY_SPACE = 32, DIS_KEY_ENTER = 13, DIS_KEY_ESC = 27,
    DIS_KEY_UP = 100, DIS_KEY_DOWN, DIS_KEY_LEFT, DIS_KEY_RIGHT,
    DIS_KEY_SHIFT = 200, DIS_KEY_CTRL
};

typedef struct {
    int key_code;
    int is_pressed;  // 1 = pressed, 0 = released
    char char_code;
} dis_key_event;

// === GRAPHICS FUNCTIONS ===
dis_graphics_ctx dis_graphics_create(uint32_t width, uint32_t height) {
    dis_graphics_ctx ctx;
    ctx.width = width;
    ctx.height = height;
    ctx.framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    printf("Graphics initialized: %ux%u\n", width, height);
    return ctx;
}

void dis_graphics_clear(dis_graphics_ctx* ctx, dis_color color) {
    for (size_t i = 0; i < ctx->width * ctx->height; i++) {
        ctx->framebuffer[i] = color;
    }
}

void dis_graphics_set_pixel(dis_graphics_ctx* ctx, int x, int y, dis_color color) {
    if (x >= 0 && x < (int)ctx->width && y >= 0 && y < (int)ctx->height) {
        ctx->framebuffer[y * ctx->width + x] = color;
    }
}

void dis_graphics_draw_line(dis_graphics_ctx* ctx, int x1, int y1, int x2, int y2, dis_color color) {
    // Bresenham line algorithm (simplified)
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (dx > dy) ? dx : dy;
    
    if (steps == 0) {
        dis_graphics_set_pixel(ctx, x1, y1, color);
        return;
    }
    
    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    
    float x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        dis_graphics_set_pixel(ctx, (int)x, (int)y, color);
        x += xInc;
        y += yInc;
    }
}

void dis_graphics_draw_rect(dis_graphics_ctx* ctx, int x, int y, int w, int h, dis_color color) {
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            dis_graphics_set_pixel(ctx, i, j, color);
        }
    }
}

void dis_graphics_draw_circle(dis_graphics_ctx* ctx, int cx, int cy, int radius, dis_color color) {
    // Simple circle rasterization
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                dis_graphics_set_pixel(ctx, cx + x, cy + y, color);
            }
        }
    }
}

void dis_graphics_present(dis_graphics_ctx* ctx) {
    // In a real implementation, this would swap buffers or update the display
    printf("Graphics present (buffer updated)\n");
}

void dis_graphics_close(dis_graphics_ctx* ctx) {
    if (ctx->framebuffer) free(ctx->framebuffer);
    printf("Graphics closed\n");
}

// Input handling
int dis_input_get_key() {
    // Stub - would read from keyboard
    return 0;
}

dis_key_event dis_input_poll_event() {
    dis_key_event evt;
    evt.key_code = DIS_KEY_UNKNOWN;
    evt.is_pressed = 0;
    evt.char_code = 0;
    return evt;
}

int dis_input_is_key_down(int key_code) {
    // Stub - would check if key is currently pressed
    return 0;
}

// Hotkey binding
typedef void (*dis_hotkey_callback)(void);

int dis_register_hotkey(int key_code, int modifiers, dis_hotkey_callback callback) {
    printf("Hotkey registered: key=%d, modifiers=%d\n", key_code, modifiers);
    return 0;
}

void dis_unregister_hotkey(int key_code, int modifiers) {
    printf("Hotkey unregistered: key=%d, modifiers=%d\n", key_code, modifiers);
}

#endif // DISPLEXITY_GRAPHICS_H
