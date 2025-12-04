/* CWE 415: Double Free (simulated custom heap) */

volatile int sink;
static char heap_area[64];
static int heap_index;
static int freed;

char *my_alloc(int n) {
    char *p;
    if (heap_index + n > 64) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void my_free(char *p) {
    /* simulate double-free detection via simple flag misuse */
    if (freed) {
        /* second free -> memory safety misuse */
        sink += 1;
    }
    freed = 1;
}

void start(void) {
    char *p = my_alloc(16);
    int i;

    freed = 0;

    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }

    my_free(p);
    my_free(p); /* double free */
}
