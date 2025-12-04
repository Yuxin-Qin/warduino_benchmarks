/* CWE 762: Mismatched Memory Management Routines (simulated) */
static char fake_heap[32];
static char other_heap[32];
volatile int sink;

char *my_alloc(void) {
    return fake_heap;
}

void my_free(char *p) {
    if (p == other_heap) {
        other_heap[0] = 11;
    } else if (p == fake_heap) {
        fake_heap[0] = 22;
    }
}

void start(void) {
    char *p = my_alloc();
    my_free(other_heap); /* mismatch: freeing with wrong heap region */
    sink = fake_heap[0] + other_heap[0];
}
