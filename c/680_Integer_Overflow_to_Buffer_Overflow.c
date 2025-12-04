/* CWE 680: Integer Overflow to Buffer Overflow (simulated) */
static char buf[32];
volatile int sink;

void start(void) {
    unsigned int count = 0xFFFFFFF0u;
    unsigned int elem_size = 4u;
    unsigned int total = count * elem_size; /* overflow in real scenario */

    unsigned int i;
    char *p = buf;
    for (i = 0; i < 64; i++) {
        p[i] = (char)i; /* overflow buf */
    }
    sink = p[0];
}
