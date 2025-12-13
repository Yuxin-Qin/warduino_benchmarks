/* CWE-119 variant 7 */
volatile int sink;
void start(void) {
    char buf[16];
    int len = 16 + 7;  /* incorrectly assumes extra room */
    for (int j = 0; j < len; j++) {
        buf[j] = (char)j;   /* write may exceed buf[15] */
    }
    sink = buf[0];
}
