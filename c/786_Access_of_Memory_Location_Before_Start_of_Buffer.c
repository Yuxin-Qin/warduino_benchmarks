/* CWE 786: Access of Memory Location Before Start of Buffer */

volatile int sink;

void start(void) {
    char buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = -4; i < 0; i++) {
        sink += buf[i]; /* out-of-bounds before start */
    }
}
