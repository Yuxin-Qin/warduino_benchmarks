/* CWE 121: Stack-based Buffer Overflow */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i <= 8; i++) {
        buf[i] = (char)(i + 1); /* write at buf[8] is out-of-bounds */
    }

    sink = buf[0];
}
