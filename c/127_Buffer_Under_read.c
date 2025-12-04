/* CWE 127: Buffer Under-read */
static char buf[16];
volatile int sink;

void start(void) {
    char *p = buf + 8;
    int i;
    int sum = 0;

    for (i = 0; i < 16; i++) {
        sum += p[-i]; /* reads before buf */
    }

    sink = sum;
}
