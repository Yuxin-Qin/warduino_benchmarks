volatile char sink;

void start(void) {
    char dest[16];
    char src[32];
    int i;

    for (i = 0; i < 32; i++) {
        src[i] = (char)i;
    }

    /* Improper restriction: copy full src into smaller dest. */
    for (i = 0; i < 32; i++) {
        dest[i] = src[i];   /* writes past dest[15] */
    }

    sink = dest[0];
}

