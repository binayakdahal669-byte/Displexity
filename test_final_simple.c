#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    printf("%s\n", "=== Displexity Final Test ===");
    char data[256] = "test";
    char result[256];
    printf("%s\n", "Crypto library: Working");
    int sum = 0;
    for (int i = 1; i < 4; i++) {
    sum = (sum + i);
    }
    printf("Loop result: %d", sum);
    if ((sum > 5)) {
    printf("%s\n", "Conditional: Working");
    } else {
    printf("%s\n", "Conditional: Error");
    }
    printf("%s\n", "=== All Systems Operational! ===");
    return 0;
}
