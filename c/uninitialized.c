// uninitialized.c
#include <stdio.h>

int main(void) {
    int x;          // uninitialized
    int y = x + 1;  // BUG: read of uninitialized variable

    printf("Uninitialized value: %d\n", y);
    return 0;
}
