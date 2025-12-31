volatile char sink;

void start(void) {
    int n = 8;
    char buf[8];
    int i;

    /* Incorrectly using element count as byte count while assuming int-size. */
    int bytes = n * (int)sizeof(int);  /* likely > 8 */

    for (i = 0; i < bytes; i++) {
        buf[i] = (char)i;   /* writes past buf[7] */
    }

    sink = buf[0];
}

