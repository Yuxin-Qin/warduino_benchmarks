volatile int sink;

static int big_buf[64];

void start(void) {
    /* Region: logical elements [16..31] inside big_buf */
    int *region = &big_buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        region[i] = i;
    }

    /* Out-of-range pointer offset: step 20 ints forward from region base.
       This lands inside big_buf but outside the logical region. */
    int *p = region + 20;
    *p = 999;

    sink = region[0];
}
