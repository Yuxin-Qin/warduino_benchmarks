/*
 * Use-after-free-like pattern with a local allocator.
 * No libc malloc/free.
 */

static char heap_area[16];
static int heap_used = 0;

static void *my_alloc(int size) {
    if (heap_used + size > (int)sizeof(heap_area)) {
        return 0;
    }
    void *p = &heap_area[heap_used];
    heap_used += size;
    return p;
}

/* Simulated free: does not actually reclaim memory. */
static void my_free(void *p) {
    (void)p;
}

volatile char sink;

int main(void) {
    char *p = (char *)my_alloc(8);
    if (!p) {
        return 0;
    }
    p[0] = 42;
    my_free(p);
    /* Use pointer after conceptual free. */
    p[1] = 13;
    sink = p[1];
    return 0;
}
