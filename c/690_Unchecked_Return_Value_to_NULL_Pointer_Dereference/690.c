/* CWE-690 */
volatile int sink;
static char heap_area[16];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 16) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void start(void) {
    char *p = my_alloc(32); /* will return NULL */
    p[0] = 1;               /* unchecked deref */
    sink = p[0];
}

