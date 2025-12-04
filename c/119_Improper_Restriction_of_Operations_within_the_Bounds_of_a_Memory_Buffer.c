/* CWE 119: Improper Restriction of Operations within the Bounds of a Memory Buffer */

volatile int sink;

void start(void) {
    char buf[16];
    char *p = buf;
    int i;

    /* Writes beyond the logical object limit (improper bounds use) */
    for (i = 0; i < 32; i++) {
        p[i] = (char)i; /* out-of-bounds for i >= 16 */
    }

    sink = p[0];
}
