/* CWE 416: Use After Free (simulated) */
static char fake_heap[32];
static int in_use = 0;
volatile int sink;

char *my_alloc(void) {
    if (in_use) return 0;
    in_use = 1;
    return fake_heap;
}

void my_free(char *p) {
    (void)p;
    in_use = 0;
}

void start(void) {
    int i;
    char *p = my_alloc();
    if (!p) return;
    for (i = 0; i < 16; i++) p[i] = (char)i;
    my_free(p);
    /* use after free */
    p[0] = 99;
    sink = p[0];
}
