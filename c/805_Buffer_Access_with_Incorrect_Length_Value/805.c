/* CWE-805 */
volatile int sink;
void start(void) {
    char buf[8];
    int len = 12; /* incorrect length */
    for (int i = 0; i < len; i++) {
        buf[i] = (char)i;   /* overflow */
    }
    sink = buf[0];
}

