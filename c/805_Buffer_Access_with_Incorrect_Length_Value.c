/* CWE 805: Buffer Access with Incorrect Length Value */
static char buf[16];
volatile int sink;

void start(void) {
    int length = 32; /* incorrect length */
    int i;
    for (i = 0; i < length; i++) {
        buf[i] = (char)i; /* out-of-bounds write */
    }
    sink = buf[0];
}
