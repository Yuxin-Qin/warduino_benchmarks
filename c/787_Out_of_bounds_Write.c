/* CWE 787: Out-of-bounds Write */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = (char)i; /* buf[8..15] out-of-bounds */
    }

    sink = buf[0];
}
