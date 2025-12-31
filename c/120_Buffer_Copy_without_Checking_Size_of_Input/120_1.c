volatile char sink;

void start(void) {
    char dst[8];
    char src[64];
    int i;

    for (i = 0; i < 64; i++) {
        src[i] = (char)(i + 1);
    }

    /* No length check on input size. */
    for (i = 0; i < 64; i++) {
        dst[i] = src[i];    /* classic buffer overflow */
    }

    sink = dst[0];
}

