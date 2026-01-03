#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    int x = 42;
    float pi = 3.14;
    char name[256] = "DISP";
    int flag = 1;
    printf("%s\n", "=== Variable Declarations ===");
    printf("%d\n", x);
    printf("%s\n", name);
    printf("Enter a number: ");
    scanf("%d", &x);
    while(getchar() != '\n');
    printf("%d\n", x);
    printf("%s\n", "=== Format Output ===");
    printf("0x%x\n", (unsigned int)x);
    { unsigned int __v = (unsigned int)x; printf("0b");
      for(int __i = 31; __i >= 0; __i--) printf("%d", (__v >> __i) & 1);
      printf("\n"); }
    printf("0o%o\n", (unsigned int)x);
    printf("%d\n", (int)x);
    x = (float)x;
    printf("%s\n", "Converted to float");
    printf("%s\n", "Done!");
    return 0;
}
