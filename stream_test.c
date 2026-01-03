#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    int x = 42;
    printf("x = %d\n", x);
    for (int i = 0; i < 3; i++) {
    printf("i = %s\n", i);
    }
    printf("%s\n", "Done\n");
    return 0;
}
