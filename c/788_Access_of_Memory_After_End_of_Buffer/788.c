/* CWE-788 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (char)i;
    char v = buf[12];   /* access after end */
    sink = (int)v;
}

