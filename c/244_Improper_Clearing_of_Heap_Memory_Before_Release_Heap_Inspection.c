/* CWE 244: Improper Clearing of Heap Memory Before Release (simulated) */
static char fake_heap[32];
static int allocated = 0;
volatile int sink;

char *my_alloc(int n) {
    if (allocated || n > 32) return 0;
    allocated = 1;
    return fake_heap;
}

void my_free(char *p) {
    (void)p;
    /* memory contains old data, not cleared */
}

void start(void) {
    int i;
    char *p = my_alloc(16);
    if (!p) return;
    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }
    my_free(p);
    /* later, read stale secret */
    sink = fake_heap[0];
}
