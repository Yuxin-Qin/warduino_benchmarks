// Intentional memory weakness: conceptual double free
// Same tiny allocator as use_after_free.c

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
    // no real reclamation
    (void)p;
}

void start() {
    char *p = (char *)my_alloc(16);
    if (!p) return;
    p[0] = 1;
    my_free(p);
    // Conceptual double free
    my_free(p);
}
