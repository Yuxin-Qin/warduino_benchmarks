/* CWE-125 variant 2 */
volatile int sink;
void start(void) {
    char buf[16];
    for (int j = 0; j < 16; j++) buf[j] = (char)j;
    /* read beyond end */
    char v = buf[16 + 2];
    sink = (int)v;
}
