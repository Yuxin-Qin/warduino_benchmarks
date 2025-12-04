/* CWE 806: Buffer Access Using Size of Source Buffer */

volatile int sink;

void start(void) {
    char src[16];
    char dst[4];
    int i;

    for (i = 0; i < 16; i++) {
        src[i] = (char)(i + 1);
    }

    /* use size of src instead of size of dst */
    for (i = 0; i < 16; i++) {
        dst[i] = src[i]; /* dst[4..15] out-of-bounds */
    }

    sink = dst[0];
}
