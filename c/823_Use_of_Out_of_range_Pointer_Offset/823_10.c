volatile int sink;

static int area[96];

void start(void) {
    int *front = &area[16];  /* [16..31] */
    int *back  = &area[32];  /* [32..47] */
    int i;

    for (i = 0; i < 16; i++) {
        front[i] = 900 + i;
        back[i]  = 1000 + i;
    }

    /* Incorrect stepping: skips by 3 instead of 1 in the front region. */
    int idx = 0;
    while (idx < 8) {
        front[idx] = 0x1234;
        idx += 3;  /* jumps beyond 16 after a few iterations */
    }

    /* Final write beyond intended region front, still in area. */
    int *p = front + 20;
    *p = 0x8888;

    sink = back[0];
}
