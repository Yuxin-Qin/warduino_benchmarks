/* CWE-761 */
static char heap_area[32];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(void *p) {
    if (p != (void *)heap_area) {
        heap_used += 4;   /* mis-handles interior pointer */
    }
}
void start(void) {
    char *base = my_alloc(16);
    if (!base) return;
    char *p = base + 4;
    my_free(p);  /* not the start of buffer */
}

