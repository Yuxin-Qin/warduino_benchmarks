/* CWE-120 */
volatile int sink;
void start(void) {
    char src[32];
    char dst[8];
    int len = 20;  /* attacker controlled */
    for (int i = 0; i < len; i++) {
        src[i] = (char)i;
    }
    /* No check that len <= sizeof(dst) */
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];  /* overflow of dst */
    }
    sink = dst[0];
}

