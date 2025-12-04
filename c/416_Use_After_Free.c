/* CWE 416: Use After Free (simulated custom heap) */

volatile int sink;
static char heap_area[64];
static int heap_index;

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
    int i;
    /* "free" by scribbling metadata */
    for (i = 0; i < 16; i++) {
        p[i] = 0;
    }
}

void start(void) {
    char *p = my_alloc(16);
    int i;

    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }

    my_free(p);

    /* use-after-free: still dereference p */
    for (i = 0; i < 16; i++) {
        sink += p[i];
    }
}
