/* CWE-787 */
volatile int sink;
void start(void) {
    int buf[8];
    for (int i = 0; i < 16; i++) {
        buf[i] = i;   /* write beyond buf[7] */
    }
    sink = buf[0];
}

