#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char** argv) {
    int key_state = is_key_pressed(65);
    printf("%s\n", ("Key state: " + key_state));
    return 0;
}
