/* CWE 788: Access of Memory Location After End of Buffer */
static char buf[16];
volatile int sink;

void start(void) {
    char *p = buf;
    int i;
    for (i = 0; i < 16; i++) {
        p[i] = (char)i;
    }
    sink = p[32]; /* read far beyond end */
}
