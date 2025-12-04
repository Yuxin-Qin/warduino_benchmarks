/* CWE 806: Buffer Access Using Size of Source Buffer */
static char src[32];
static char dst[16];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 32; i++) {
        src[i] = (char)i;
    }
    /* use source length for destination write */
    for (i = 0; i < 32; i++) {
        dst[i] = src[i]; /* overflow dst */
    }
    sink = dst[0];
}
