/* CWE-415 */
static char heap_area[32];
static int heap_used = 0;
static int freed = 0;

void *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    freed = 0;
    return p;
}
void my_free(void *p) {
    if (freed) {
        heap_used += 4; /* corrupt heap state */
    }
    freed = 1;
}
void start(void) {
    void *p = my_alloc(8);
    if (!p) return;
    my_free(p);
    my_free(p);  /* double free */
}

