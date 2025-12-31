volatile int sink;

void start(void) {
    int buf[16];
    int *p = &buf[4];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Access before start of buffer using negative index. */
    for (i = -4; i < 4; i++) {
        int v = p[i];   /* for i < 0, p[i] is before buf[0] */
        sink = v;
    }
}

