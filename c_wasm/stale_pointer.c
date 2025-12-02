/*
 * Stale pointer pattern using a simple bump allocator.
 * No real free, but simulates reusing a region.
 */

static char heap_area[32];
static int heap_offset = 0;

static void *my_alloc(int size) {
    if (heap_offset + size > (int)sizeof(heap_area)) {
        return 0;
    }
    void *p = &heap_area[heap_offset];
    heap_offset += size;
    return p;
}

/* No-op free; we just simulate conceptual lifetime issues. */
static void my_free(void *p) {
    (void)p;
}

volatile char sink;

int main(void) {
    char *p1 = (char *)my_alloc(8);
    char *p2;

    p1[0] = 1;
    my_free(p1); /* conceptually freed */

    /* Allocate again from the same pool */
    p2 = (char *)my_alloc(8);
    p2[0] = 2;

    /* Stale pointer p1 still used */
    sink = p1[0];
    return 0;
}
