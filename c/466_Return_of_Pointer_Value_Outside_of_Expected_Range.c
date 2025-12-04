/* CWE 466: Return of Pointer Value Outside of Expected Range (simulated) */
static char buf[16];
volatile int sink;

char *get_ptr_outside(void) {
    return buf + 32; /* outside expected region */
}

void start(void) {
    char *p = get_ptr_outside();
    p[0] = 7; /* invalid location */
    sink = buf[0];
}
