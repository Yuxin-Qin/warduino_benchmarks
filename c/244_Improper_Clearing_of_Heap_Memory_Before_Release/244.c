/* CWE-244 */
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
    for (int i = 0; i < n - 4; i++) { /* tails not cleared */
        p[i] = 0;
    }
}
void start(void) {
    char *p = my_alloc(16);
    if (!p) return;
    for (int i = 0; i < 16; i++) p[i] = (char)(0x60 + i);
    my_free(p, 16);
    sink = p[15];  /* may still hold 'secret' */
}

