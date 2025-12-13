/* CWE-823 */
volatile int sink;
void start(void) {
    int buf[16];
    int *p = &buf[0];
    int *q = p + 32;  /* out-of-range offset */
    *q = 99;
    sink = buf[0];
}

