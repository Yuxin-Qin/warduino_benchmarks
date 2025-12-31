volatile int sink;

void start(void) {
    int table[10];
    int i;

    for (i = 0; i < 10; i++) {
        table[i] = i;
    }

    /* Unvalidated index far beyond array size. */
    int idx = 25;

    table[idx] = 999;  /* no bounds check at all */
    sink = table[0];
}

