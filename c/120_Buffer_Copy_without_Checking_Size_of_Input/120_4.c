/* CWE-120 variant 4 */
volatile int sink;
void start(void) {
    char src[32];
    char dst[8];
    int len = 16 + 4; /* attacker controlled length */
    for (int k = 0; k < len; k++) {
        src[k] = (char)k;
    }
    /* No check that len <= sizeof(dst) */
    for (int k = 0; k < len; k++) {
        dst[k] = src[k];
    }
    sink = dst[0];
}
