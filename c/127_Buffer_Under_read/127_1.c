volatile int sink;

void start(void) {
    int buf[16];
    int *p = &buf[8];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Under-read: for i < 8, we read before buf[0]. */
    for (i = 0; i < 16; i++) {
        int v = p[i - 8];  /* buf[-8]..buf[7] */
        sink = v;
    }
}

