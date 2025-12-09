volatile int sink;

static int arena[64];

void start(void) {
    /* Two logical regions inside one arena */
    int *region_a = &arena[0];   /* [0..15]   */
    int *region_b = &arena[16];  /* [16..31]  */
    int i;

    for (i = 0; i < 16; i++) {
        region_a[i] = i;
        region_b[i] = 100 + i;
    }

    /* Bug: code thinks region_a has length 20 and writes into region_b. */
    int *p = region_a + 18;
    *p = 0x1111;

    sink = region_b[0];
}
