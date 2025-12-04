/* CWE 763: Release of Invalid Pointer or Reference (simulated) */
static char fake_heap[32];
volatile int sink;

void my_free(char *p) {
    if (p < fake_heap || p >= fake_heap + 32) {
        fake_heap[0] = 55; /* invalid pointer */
    }
}

void start(void) {
    char *p = fake_heap + 64; /* definitely outside */
    my_free(p);
    sink = fake_heap[0];
}
