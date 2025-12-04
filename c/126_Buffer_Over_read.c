/* CWE 126: Buffer Over-read */
static char src[8] = {1,2,3,4,5,6,7,8};
static char dst[8];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 16; i++) {
        dst[i] = src[i]; /* over-read src */
    }
    sink = dst[0];
}
