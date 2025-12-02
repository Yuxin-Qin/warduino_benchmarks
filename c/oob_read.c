// oob_read.c
#include <stdio.h>

int main(void) {
    char buf[16];
    for (int i = 0; i < 16; i++) {
        buf[i] = (char)('a' + i);
    }

    // BUG: read past the end
    char x = buf[16];
    printf("OOB read value: %d\n", (int)x);
    return 0;
}
