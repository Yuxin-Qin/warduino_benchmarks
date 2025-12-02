// misaligned_access.c
#include <stdio.h>
#include <stdint.h>

int main(void) {
    // Force a misaligned pointer (on some architectures this will trap;
    // on others it's UB but may appear to work).
    uint8_t buf[sizeof(int) + 1];

    uint8_t *p = buf + 1;   // likely misaligned for int
    int *ip = (int *)p;     // BUG: misaligned + type-pun abuse

    *ip = 0x11223344;       // UB
    printf("Wrote 0x%x via misaligned int*\n", *ip);
    return 0;
}
