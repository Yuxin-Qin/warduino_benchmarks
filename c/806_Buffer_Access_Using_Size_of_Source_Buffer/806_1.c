volatile int sink;

void start(void) {
    int src[32];
    int dst[8];
    int i;

    for (i = 0; i < 32; i++) {
        src[i] = i;
    }

    /* Uses size of source array to index destination. */
    int src_size = 32;

    for (i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* out-of-bounds for i >= 8 */
    }

    sink = dst[0];
}

