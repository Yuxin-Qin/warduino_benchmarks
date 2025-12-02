// buffer_overflow.c
#include <stdio.h>

int main(void) {
    char buf[16];

    for (int i = 0; i <= 16; i++) {  // BUG: writes one past the end
        buf[i] = 'A';
    }

    printf("buf[0] = %c\n", buf[0]);
    return 0;
}
