/* CWE 120: Buffer Copy without Checking Size of Input (Classic Buffer Overflow) */
static char dst[16];
static char src[32];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 32; i++) {
        src[i] = (char)i;
    }

    /* naive copy: assumes dst can hold 32 bytes */
    for (i = 0; i < 32; i++) {
        dst[i] = src[i]; /* overflow dst */
    }

    sink = dst[0];
}
