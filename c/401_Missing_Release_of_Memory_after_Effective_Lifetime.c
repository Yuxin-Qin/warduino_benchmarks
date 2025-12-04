/* CWE 401: Missing Release of Memory after Effective Lifetime (simulated leak) */
static char fake_heap[64];
static int offset = 0;
volatile int sink;

char *my_alloc(int n) {
    if (offset + n > 64) return 0;
    char *p = fake_heap + offset;
    offset += n;
    return p;
}

void start(void) {
    int i;
    char *p = my_alloc(32);
    if (!p) return;
    for (i = 0; i < 32; i++) {
        p[i] = (char)i;
    }
    /* never freed, leak simulated */
    sink = p[0];
}
