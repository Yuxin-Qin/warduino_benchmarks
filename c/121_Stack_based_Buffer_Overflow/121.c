/* CWE-121 */
volatile int guard = 12345;
void start(void) {
    int buf[8];
    for (int i = 0; i < 16; i++) {
        buf[i] = 0x1000 + i;  /* writes beyond buf[7] */
    }
    (void)guard;
}

