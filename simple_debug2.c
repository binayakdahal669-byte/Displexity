#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// DEBUG: emitRuntimeHelpers called
// Comprehensive Input System
#ifdef _WIN32
#include <windows.h>
static POINT __disp_mouse_pos = {0, 0};
static bool __disp_keys[256] = {false};
static bool __disp_mouse_buttons[8] = {false};
static int __disp_mouse_wheel = 0;

void __disp_update_input() {
    // Update mouse position
    GetCursorPos(&__disp_mouse_pos);
    if (__disp_hwnd) ScreenToClient(__disp_hwnd, &__disp_mouse_pos);
    
    // Update key states
    for (int i = 0; i < 256; i++) {
        __disp_keys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
    
    // Update mouse buttons
    __disp_mouse_buttons[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    __disp_mouse_buttons[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    __disp_mouse_buttons[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
    __disp_mouse_buttons[3] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
    __disp_mouse_buttons[4] = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
}

int __disp_is_key_pressed(int key_code) {
    __disp_update_input();
    return __disp_keys[key_code] ? 1 : 0;
}

int __disp_is_key_just_pressed(int key_code) {
    static bool prev_keys[256] = {false};
    __disp_update_input();
    bool result = __disp_keys[key_code] && !prev_keys[key_code];
    prev_keys[key_code] = __disp_keys[key_code];
    return result ? 1 : 0;
}

int __disp_is_key_released(int key_code) {
    static bool prev_keys[256] = {false};
    __disp_update_input();
    bool result = !__disp_keys[key_code] && prev_keys[key_code];
    prev_keys[key_code] = __disp_keys[key_code];
    return result ? 1 : 0;
}

int __disp_get_mouse_x() {
    __disp_update_input();
    return __disp_mouse_pos.x;
}

int __disp_get_mouse_y() {
    __disp_update_input();
    return __disp_mouse_pos.y;
}

int __disp_is_mouse_pressed(int button) {
    __disp_update_input();
    return (button >= 0 && button < 8) ? __disp_mouse_buttons[button] : 0;
}

int __disp_is_mouse_just_pressed(int button) {
    static bool prev_mouse[8] = {false};
    __disp_update_input();
    if (button < 0 || button >= 8) return 0;
    bool result = __disp_mouse_buttons[button] && !prev_mouse[button];
    prev_mouse[button] = __disp_mouse_buttons[button];
    return result ? 1 : 0;
}

int __disp_is_mouse_released(int button) {
    static bool prev_mouse[8] = {false};
    __disp_update_input();
    if (button < 0 || button >= 8) return 0;
    bool result = !__disp_mouse_buttons[button] && prev_mouse[button];
    prev_mouse[button] = __disp_mouse_buttons[button];
    return result ? 1 : 0;
}

void __disp_set_mouse_position(int x, int y) {
    if (__disp_hwnd) {
        POINT pt = {x, y};
        ClientToScreen(__disp_hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
    }
}

void __disp_show_cursor(int show) {
    ShowCursor(show ? TRUE : FALSE);
}

int __disp_get_mouse_wheel() {
    int result = __disp_mouse_wheel;
    __disp_mouse_wheel = 0;  // Reset after reading
    return result;
}

// Key code constants
#define DISP_KEY_SPACE 32
#define DISP_KEY_ENTER 13
#define DISP_KEY_ESC 27
#define DISP_KEY_TAB 9
#define DISP_KEY_BACKSPACE 8
#define DISP_KEY_DELETE 46
#define DISP_KEY_LEFT 37
#define DISP_KEY_UP 38
#define DISP_KEY_RIGHT 39
#define DISP_KEY_DOWN 40
#define DISP_KEY_SHIFT 16
#define DISP_KEY_CTRL 17
#define DISP_KEY_ALT 18
#define DISP_KEY_F1 112
#define DISP_KEY_F2 113
#define DISP_KEY_F3 114
#define DISP_KEY_F4 115
#define DISP_KEY_F5 116
#define DISP_KEY_F6 117
#define DISP_KEY_F7 118
#define DISP_KEY_F8 119
#define DISP_KEY_F9 120
#define DISP_KEY_F10 121
#define DISP_KEY_F11 122
#define DISP_KEY_F12 123
#define DISP_MOUSE_LEFT 0
#define DISP_MOUSE_RIGHT 1
#define DISP_MOUSE_MIDDLE 2
#define DISP_MOUSE_X1 3
#define DISP_MOUSE_X2 4

#else
// Linux input stubs
void __disp_update_input() {}
int __disp_is_key_pressed(int key_code) { return 0; }
int __disp_is_key_just_pressed(int key_code) { return 0; }
int __disp_is_key_released(int key_code) { return 0; }
int __disp_get_mouse_x() { return 0; }
int __disp_get_mouse_y() { return 0; }
int __disp_is_mouse_pressed(int button) { return 0; }
int __disp_is_mouse_just_pressed(int button) { return 0; }
int __disp_is_mouse_released(int button) { return 0; }
void __disp_set_mouse_position(int x, int y) {}
void __disp_show_cursor(int show) {}
int __disp_get_mouse_wheel() { return 0; }
#endif

int __disp_get_keypress() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

char* __disp_keypress_to_string(int key) {
    static char buf[2] = {0, 0};
    buf[0] = (char)key;
    return buf;
}

typedef void (*__disp_hotkey_fn)();
struct { int key; __disp_hotkey_fn fn; } __disp_hotkeys[256];
int __disp_hotkey_count = 0;

void __disp_register_hotkey(int key, __disp_hotkey_fn fn) {
    __disp_hotkeys[__disp_hotkey_count].key = key;
    __disp_hotkeys[__disp_hotkey_count].fn = fn;
    __disp_hotkey_count++;
}



int main(int argc, char** argv) {
    printf("%s\n", "Hello World");
    return 0;
}
