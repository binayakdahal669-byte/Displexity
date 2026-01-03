#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    struct { char str[256]; int ival; float fval; } c = {"FF", 0, 0.0f};
    int x = 42;
    char name[256] = "World";
    printf("Hello %s!\n", name);
    printf("x = %d\n", x);
    long decv;
    decv = strtol(c.str, NULL, 16);
    printf("Hex value: %d\n", decv);
    { unsigned int __v = (unsigned int)decv; printf("0b");
      for(int __i = 31; __i >= 0; __i--) printf("%d", (__v >> __i) & 1);
      printf("\n"); }
    printf("0x%x\n", (unsigned int)decv);
    printf("0o%o\n", (unsigned int)decv);
    printf("%d\n", (int)decv);
    printf("%s\n", "Done!\n");
    return 0;
}
