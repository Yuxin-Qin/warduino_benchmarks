// Intentional memory weakness: conceptual use-after-free
// We simulate a tiny heap and a dummy "free" to keep everything self-contained.

static char heap[64];
static int used = 0;

static void *my_alloc(int sz) {
    if (used + sz > (int)sizeof(heap)) {
        return 0;
    }
    void *p = &heap[used];
    used += sz;
    return p;
}

static void my_free(void *p) {
    // no real reclamation; just a placeholder
    (void)p;
}

void start() {
    char *p = (char *)my_alloc(16);
    if (!p) return;
    for (int i = 0; i < 16; i++) {
        p[i] = (char)i;
    }
    my_free(p);
    // Conceptual use-after-free: keep writing through the pointer
    p[0] = 42;
    p[15] = 99;
}
