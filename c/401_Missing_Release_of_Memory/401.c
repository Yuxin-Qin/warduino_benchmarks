/* CWE-401 */
static char heap_area[128];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 128) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void start(void) {
    for (int r = 0; r < 16; r++) {
        (void)my_alloc(8); /* never freed */
    }
}

