/* CWE 120: Buffer Copy without Checking Size of Input */

volatile int sink;

void start(void) {
    char src[64];
    char dst[16];
    int i;

    for (i = 0; i < 64; i++) {
        src[i] = (char)i;
    }

    /* Copy more bytes than destination can hold */
    for (i = 0; i < 64; i++) {
        dst[i] = src[i]; /* out-of-bounds for i >= 16 */
    }

    sink = dst[0];
}
