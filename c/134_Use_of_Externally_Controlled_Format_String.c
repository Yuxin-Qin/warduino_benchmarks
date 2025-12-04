/* CWE 134: Use of Externally-Controlled Format String (simulated) */
static char fmt[8] = {'%','x','%','x','%','x','%','x'};
static char buf[8];
volatile int sink;

void start(void) {
    int i;
    /* naive copy using untrusted "format" as length */
    for (i = 0; i < 32; i++) {
        buf[i] = fmt[i]; /* overflow buf */
    }
    sink = buf[0];
}
