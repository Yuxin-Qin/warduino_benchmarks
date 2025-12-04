/* CWE 761: Free of Pointer not at Start of Buffer (simulated) */
static char fake_heap[32];
volatile int sink;

void my_free(char *p) {
    /* expect exact base */
    if (p != fake_heap) {
        fake_heap[0] = 77; /* signal wrong free */
    }
}

void start(void) {
    char *base = fake_heap;
    char *mid = base + 8;
    my_free(mid); /* incorrect pointer passed to free */
    sink = fake_heap[0];
}
