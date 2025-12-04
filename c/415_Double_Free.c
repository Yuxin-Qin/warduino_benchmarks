/* CWE 415: Double Free (simulated) */
static char fake_heap[32];
static int freed = 0;
volatile int sink;

char *my_alloc(void) {
    if (freed) return 0;
    return fake_heap;
}

void my_free(char *p) {
    (void)p;
    if (freed) {
        /* double free state */
        fake_heap[0] = 42; /* scribble */
    }
    freed = 1;
}

void start(void) {
    char *p = my_alloc();
    if (!p) return;
    my_free(p);
    my_free(p); /* double free */
    sink = fake_heap[0];
}
