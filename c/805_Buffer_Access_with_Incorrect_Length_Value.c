/* CWE 805: Buffer Access with Incorrect Length Value */

volatile int sink;

void start(void) {
    char buf[8];
    int length = 16; /* incorrect length */
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = 0; i < length; i++) {
        sink += buf[i]; /* out-of-bounds for i >= 8 */
    }
}
