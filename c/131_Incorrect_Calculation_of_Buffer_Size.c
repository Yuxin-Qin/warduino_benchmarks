/* CWE 131: Incorrect Calculation of Buffer Size */

volatile int sink;

void start(void) {
    char buf[10];
    int count = 10;
    int i;

    /* Intended to allocate 10 bytes but uses 10 elements of 2 bytes each conceptually */
    for (i = 0; i < count * 2; i++) {
        buf[i] = (char)i; /* buf[10..19] out-of-bounds */
    }

    sink = buf[0];
}
