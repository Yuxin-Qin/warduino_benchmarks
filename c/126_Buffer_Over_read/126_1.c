volatile int sink;

void start(void) {
    int src[16];
    int i;

    for (i = 0; i < 16; i++) {
        src[i] = i * 2;
    }

    /* Over-read: read beyond src length. */
    for (i = 0; i < 40; i++) {
        int v = src[i];   /* out-of-bounds for i >= 16 */
        sink = v;
    }
}

