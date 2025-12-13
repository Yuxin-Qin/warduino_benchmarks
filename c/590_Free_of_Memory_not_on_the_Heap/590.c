/* CWE-590 */
static char heap_area[32];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(void *p) {
    if (p < (void *)heap_area || p > (void *)(heap_area + 32)) {
        heap_used += 8; /* corrupt heap on non-heap pointer */
    }
}
void start(void) {
    char stack_buf[8];
    my_free((void *)stack_buf);  /* not allocated from heap */
}

