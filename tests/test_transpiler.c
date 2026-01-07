// Test: C to Displexity Transpiler Input
// This C file should be transpiled to valid Displexity code

#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    printf("C to Displexity Transpiler Test\n");
    
    int x = 10;
    int y = 20;
    int sum = add(x, y);
    
    printf("Sum: %d\n", sum);
    
    for (int i = 1; i <= 5; i++) {
        int fact = factorial(i);
        printf("Factorial of %d is %d\n", i, fact);
    }
    
    if (sum > 25) {
        printf("Sum is greater than 25\n");
    } else {
        printf("Sum is not greater than 25\n");
    }
    
    return 0;
}
