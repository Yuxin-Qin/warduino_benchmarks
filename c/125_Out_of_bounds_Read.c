/* CWE 125: Out-of-bounds Read */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(10 + i);
    }

    /* Read past the end */
    for (i = 0; i < 16; i++) {
        sink += buf[i]; /* buf[8..15] are out-of-bounds */
    }
}
