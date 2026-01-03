#include <stdio.h>
#include <string.h>
#include <stdlib.h>




// === Linked Libraries ===
// --- Library: examples\lib/math.disll ---
int abs(int x);
int max(int a, int b);
int min(int a, int b);
int pow(int base, int exp);
int factorial(int n);
int fib(int n);

int abs(int x) {
    if ((x < 0)) {
    return (0 - x);
    }
    return x;
    return 0;
}

int max(int a, int b) {
    if ((a > b)) {
    return a;
    }
    return b;
    return 0;
}

int min(int a, int b) {
    if ((a < b)) {
    return a;
    }
    return b;
    return 0;
}

int pow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
    result = (result * base);
    }
    return result;
    return 0;
}

int factorial(int n) {
    if ((n <= 1)) {
    return 1;
    }
    int result = 1;
    for (int i = 2; i < (n + 1); i++) {
    result = (result * i);
    }
    return result;
    return 0;
}

int fib(int n) {
    if ((n <= 1)) {
    return n;
    }
    int a = 0;
    int b = 1;
    for (int i = 2; i < (n + 1); i++) {
    int temp = (a + b);
    a = b;
    b = temp;
    }
    return b;
    return 0;
}



// === End Libraries ===

int main(int argc, char** argv) {
    printf("%s\n", "Testing math library");
    int result = abs(-(5));
    printf("%d\n", result);
    return 0;
}
