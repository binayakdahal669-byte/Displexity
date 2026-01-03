#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    printf("%s\n", "=== Typed Input Demo ===");
    printf("Enter your name: ");
    char name[256];
    fgets(name, 256, stdin);
    name[strcspn(name, "\n")] = 0;
    printf("%s\n", name);
    printf("Enter your age: ");
    int age;
    scanf("%d", &age);
    while(getchar() != '\n');
    printf("%d\n", age);
    printf("Enter a decimal: ");
    float decimal;
    scanf("%f", &decimal);
    while(getchar() != '\n');
    printf("Are you happy? (yes/no): ");
    int happy;
    { char __tmp[16]; fgets(__tmp, 16, stdin); __tmp[strcspn(__tmp, "\n")] = 0;
      happy = (__tmp[0] == '1' || __tmp[0] == 't' || __tmp[0] == 'T' || __tmp[0] == 'y' || __tmp[0] == 'Y'); }
    printf("%s\n", "Done!");
    return 0;
}
