volatile int sink;

static int pool[64];

void start(void) {
    /* Logical region starts at pool[24], length 8: [24..31] */
    int *region = &pool[24];
    int i;

    for (i = 0; i < 8; i++) {
        region[i] = 200 + i;
    }

    /* Out-of-range pointer offset: step backwards beyond region start.
       This still stays inside 'pool' but violates region bounds. */
    int *p = region - 4;
    *p = 0x2222;

    sink = region[0];
}
