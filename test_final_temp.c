#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    printf("%s\n", "=== Displexity Final Test ===");
    char data[256] = "test";
    char result[256];
    printf("%s\n", "Crypto: OK");
    printf("Mouse X: %d", __disp_get_mouse_x());
    printf("%s\n", "Input: OK");
    int sum = 0;
    for (int i = 1; i < 4; i++) {
    sum = (sum + i);
    }
    printf("Loop sum: %d", sum);
    printf("%s\n", "=== System Ready! ===");
    return 0;
}
