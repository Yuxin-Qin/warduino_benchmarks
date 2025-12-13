/* CWE-825 */
volatile int sink;
static char heap_area[32];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(char *p, int n) {
    for (int i = 0; i < n; i++) p[i] = (char)0xAA;
}
void start(void) {
    char *p = my_alloc(16);
    if (!p) return;
    for (int i = 0; i < 16; i++) p[i] = (char)i;
    my_free(p, 16);
    p[4] = 7;  /* expired pointer dereference */
    sink = p[0];
}

