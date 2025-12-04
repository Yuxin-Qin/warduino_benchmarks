/* CWE 690: Unchecked Return Value to NULL Pointer Dereference */

volatile int sink;
static char heap_area[32];
static int heap_index;

char *may_fail_alloc(int n) {
    if (heap_index + n > 32) {
        return 0;
    }
    heap_index += n;
    return &heap_area[heap_index - n];
}

void start(void) {
    char *p;

    heap_index = 0;

    p = may_fail_alloc(40); /* fails, returns NULL */
    /* unchecked */
    p[0] = 1; /* NULL deref */
    sink = p[0];
}
