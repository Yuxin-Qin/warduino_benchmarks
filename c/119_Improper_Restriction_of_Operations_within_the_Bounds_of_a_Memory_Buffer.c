/* CWE 119: Improper Restriction of Operations within the Bounds of a Memory Buffer */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    int len = 32; /* incorrect bound */
    char *p = buf;

    for (i = 0; i < len; i++) {
        p[i] = (char)i; /* writes past buf[15] */
    }

    sink = p[0];
}
