/* CWE 122: Heap-based Buffer Overflow (simulated tiny heap) */
static char fake_heap[32];
volatile int sink;

char *my_alloc(int n) {
    if (n > 32) {
        return 0;
    }
    return fake_heap;
}

void start(void) {
    char *p = my_alloc(16);
    int i;

    if (!p) {
        return;
    }

    for (i = 0; i < 40; i++) {
        p[i] = (char)i; /* overflow fake_heap */
    }

    sink = p[0];
}
