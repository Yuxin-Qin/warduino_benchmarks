volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = 0;
    }

    /* Out-of-bounds write beyond end of array. */
    for (i = 0; i < 40; i++) {
        buf[i] = i;    /* writes out of bounds when i >= 16 */
    }

    sink = buf[0];
}

