/* CWE 590: Free of Memory not on the Heap (simulated) */
static char fake_heap[32];
volatile int sink;

void my_free(char *p) {
    /* if p not in fake_heap, scribble to show error */
    if (p < fake_heap || p >= fake_heap + 32) {
        fake_heap[0] = 99;
    }
}

void start(void) {
    char local[16];
    my_free(local); /* freeing non-heap */
    sink = fake_heap[0];
}
