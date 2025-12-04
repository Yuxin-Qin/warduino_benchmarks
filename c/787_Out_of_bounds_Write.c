/* CWE 787: Out-of-bounds Write */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 32; i++) {
        buf[i] = (char)i; /* write beyond 16 bytes */
    }
    sink = buf[0];
}
