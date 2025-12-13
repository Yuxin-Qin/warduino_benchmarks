/* CWE-789 */
volatile int sink;
static char heap_area[64];
static int heap_used = 0;

char *my_alloc(unsigned int n) {
    if (n > 1000u) return 0;  /* weak limit */
    if (heap_used + (int)n > 64) return 0;
    char *p = &heap_area[heap_used];
    heap_used += (int)n;
    return p;
}
void start(void) {
    unsigned int huge = 512u;
    char *p = my_alloc(huge);  /* logically excessive */
    if (!p) return;
    p[0] = 1;
    sink = p[0];
}

