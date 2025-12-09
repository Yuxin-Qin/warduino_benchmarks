volatile int sink;

static int storage[128];

void start(void) {
    /* Region is supposed to be indices [40..55], length 16 */
    int *region = &storage[40];
    int logical_len = 16;
    int i;

    for (i = 0; i < logical_len; i++) {
        region[i] = 600 + i;
    }

    /* Off-by-chunk bug: multiplies index by 2 instead of 1. */
    int bad_index = 10 * 2;  /* 20, valid index would be < 16 */
    int *p = region + bad_index;
    *p = 0x5555;

    sink = region[0];
}
