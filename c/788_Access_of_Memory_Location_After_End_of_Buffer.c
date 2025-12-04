/* CWE 788: Access of Memory Location After End of Buffer */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = 8; i < 16; i++) {
        sink += buf[i]; /* accesses past end */
    }
}
