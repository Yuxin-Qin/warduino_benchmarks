/*
 * Double-free-like pattern with a simple allocator.
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

static int freed_flag = 0;

static void my_free(void *p) {
    (void)p;
    if (freed_flag) {
        /* second free on same conceptual object */
        /* in a real allocator this would be a bug */
    }
    freed_flag = 1;
}

int main(void) {
    char *p = (char *)my_alloc(8);
    if (!p) {
        return 0;
    }
    my_free(p);
    my_free(p); /* double free pattern */
    return 0;
}
