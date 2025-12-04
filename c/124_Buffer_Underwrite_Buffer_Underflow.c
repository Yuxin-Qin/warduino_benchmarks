/* CWE 124: Buffer Underwrite (Buffer Underflow) */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    char *p = buf + 4;

    for (i = 0; i < 16; i++) {
        p[-i] = (char)i; /* writes before buf */
    }

    sink = buf[0];
}
