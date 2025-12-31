volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Access well after end of buffer. */
    for (i = 16; i < 32; i++) {
        int v = buf[i];  /* beyond buf[15] */
        sink = v;
    }
}

