volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Out-of-bounds read well past end. */
    for (i = 0; i < 32; i++) {
        sink = buf[i];  /* reads undefined memory for i >= 16 */
    }
}

