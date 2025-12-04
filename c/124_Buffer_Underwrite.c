/* CWE 124: Buffer Underwrite (Buffer Underflow) */

volatile int sink;

void start(void) {
    char buf[16];
    char *p = buf + 4;
    int i;

    /* Write before the start of the buffer */
    for (i = -4; i < 8; i++) {
        p[i] = (char)i; /* p[-4]..p[-1] underflow buf */
    }

    sink = buf[0];
}
