/* CWE-122 */
volatile int sink;
static char heap_area[32];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}

void start(void) {
    char *p = (char *)my_alloc(8);
    if (!p) return;
    for (int i = 0; i < 16; i++) {
        p[i] = (char)i;   /* overflow past allocated 8 bytes */
    }
    sink = p[0];
}

