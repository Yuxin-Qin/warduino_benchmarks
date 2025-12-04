/* CWE 789: Memory Allocation with Excessive Size Value (simulated) */
static char fake_heap[64];
volatile int sink;

char *my_alloc(unsigned int n) {
    if (n > 64u) {
        return 0; /* pretend failure */
    }
    return fake_heap;
}

void start(void) {
    unsigned int n = 1000u;
    char *p = my_alloc(n); /* returns 0 */
    if (!p) {
        p = (char *)0;
    }
    p[0] = 1; /* NULL deref */
    sink = p[0];
}
