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

// Keypress runtime
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
    int key_result = __disp_get_keypress();
    int key_state = is_key_pressed(65);
    printf("%s\n", ("Key state: " + key_state));
    return 0;
}
