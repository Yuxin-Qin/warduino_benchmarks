/* CWE 823: Use of Out-of-range Pointer Offset */
static char buf[16];
volatile int sink;

void start(void) {
    char *p = buf + 4;
    char *q = p + 32; /* out-of-range offset */
    q[0] = 9;
    sink = buf[0];
}
