/* CWE-124 variant 10 */
volatile int sink;
void start(void) {
    char buf[16];
    char *p = &buf[10];  /* some interior pointer */
    /* Underflow by writing before buf[0] */
    for (int j = 0; j < 8; j++) {
        p[-(j+1)] = (char)j;
    }
    sink = buf[0];
}
