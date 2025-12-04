/* CWE 126: Buffer Over-read */

volatile int sink;

void start(void) {
    char buf[4];
    int i;

    for (i = 0; i < 4; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = 0; i < 10; i++) {
        sink += buf[i]; /* buf[4..9] are out-of-bounds */
    }
}
