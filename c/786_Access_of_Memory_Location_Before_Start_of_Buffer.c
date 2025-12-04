/* CWE 786: Access of Memory Location Before Start of Buffer */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    char *p = buf;
    for (i = 1; i <= 8; i++) {
        p[-i] = (char)i; /* before buf */
    }
    sink = buf[0];
}
