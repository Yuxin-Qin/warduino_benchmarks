/* CWE 825: Expired Pointer Dereference */

volatile int sink;
static char heap_area[32];
static int heap_index;

char *my_alloc(int n) {
    char *p;
    if (heap_index + n > 32) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void my_reset_heap(void) {
    heap_index = 0;
}

void start(void) {
    char *p;
    int i;

    my_reset_heap();
    p = my_alloc(16);

    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }

    my_reset_heap(); /* invalidates previous allocation logically */

    /* expired pointer dereference */
    for (i = 0; i < 16; i++) {
        sink += p[i];
    }
}
