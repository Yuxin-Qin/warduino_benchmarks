/* CWE 789: Memory Allocation with Excessive Size Value (simulated) */

volatile int sink;
static char heap_area[64];

char *my_alloc(int n) {
    if (n > 64) {
        return 0;
    }
    return heap_area;
}

void start(void) {
    char *p = my_alloc(1000); /* excessive request */
    int i;

    if (!p) {
        /* still attempt to use it as if allocation succeeded */
        p = (char *)0;
    }

    for (i = 0; i < 16; i++) {
        p[i] = (char)i; /* may deref NULL */
    }

    sink = p[0];
}
