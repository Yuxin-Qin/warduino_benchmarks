/* CWE-122 variant 3 */
volatile int sink;
static char heap_area[64];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 64) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}

void start(void) {
    char *p = (char *)my_alloc(8);
    if (!p) return;
    /* write beyond allocated 8 bytes */
    for (int j = 0; j < 8 + 3; j++) {
        p[j] = (char)j;
    }
    sink = p[0];
}
