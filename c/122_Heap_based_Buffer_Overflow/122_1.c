volatile int sink;
static int fake_heap[16];

static int *my_alloc(int n) {
    /* Very naive allocator returning base of fake_heap. */
    (void)n;
    return fake_heap;
}

void start(void) {
    int *p = my_alloc(16);
    int i;

    /* Correct writes. */
    for (i = 0; i < 16; i++) {
        p[i] = i;
    }

    /* Heap-based overflow: write far beyond allocated 16 ints. */
    for (i = 16; i < 40; i++) {
        p[i] = i * 3;
    }

    sink = fake_heap[0];
}

