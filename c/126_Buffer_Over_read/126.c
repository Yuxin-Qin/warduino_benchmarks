/* CWE-126 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (char)i;
    char last = 0;
    for (int i = 0; i < 16; i++) {
        last = buf[i];  /* reads past end */
    }
    sink = (int)last;
}

