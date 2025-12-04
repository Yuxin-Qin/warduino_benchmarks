/* CWE 127: Buffer Under-read */

volatile int sink;

void start(void) {
    char buf[8];
    char *p = buf + 4;
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(i + 10);
    }

    for (i = -4; i < 4; i++) {
        sink += p[i]; /* p[-4..-1] read before buf */
    }
}
