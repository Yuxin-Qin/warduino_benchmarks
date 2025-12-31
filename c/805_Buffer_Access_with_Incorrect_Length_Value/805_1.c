volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Incorrect length (too large) used for iteration. */
    int length = 64;

    for (i = 0; i < length; i++) {
        int v = buf[i];   /* out-of-bounds for i >= 16 */
        sink = v;
    }
}

