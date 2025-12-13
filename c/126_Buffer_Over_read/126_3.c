/* CWE-126 variant 3 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int j = 0; j < 8; j++) buf[j] = (char)j;
    /* simulate copying too many bytes from buf */
    char tmp;
    for (int j = 0; j < 8 + 3; j++) {
        tmp = buf[j];  /* may read past end */
    }
    sink = (int)tmp;
}
