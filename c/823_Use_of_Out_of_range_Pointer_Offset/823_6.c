volatile int sink;

static int arena[96];

void start(void) {
    int *region1 = &arena[8];   /* [8..23]   */
    int *region2 = &arena[24];  /* [24..39]  */
    int i;

    for (i = 0; i < 16; i++) {
        region1[i] = 400 + i;
        region2[i] = 500 + i;
    }

    /* Code assumes region1 is length 24 and marches into region2. */
    int *p = region1 + 20;  /* inside arena, outside intended region1 */
    *p = 0x4444;

    sink = region2[0];
}
