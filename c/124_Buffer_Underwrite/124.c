/* CWE-124 */
volatile int sink;
void start(void) {
    char buf[16];
    char *p = &buf[4];
    for (int i = 0; i < 8; i++) {
        p[-(i+1)] = (char)i;  /* write before buf[0] */
    }
    sink = buf[0];
}

