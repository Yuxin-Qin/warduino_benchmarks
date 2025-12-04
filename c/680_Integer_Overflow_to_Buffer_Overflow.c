/* CWE 680: Integer Overflow to Buffer Overflow */

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

void start(void) {
    unsigned int count = 0x80000001u; /* huge count */
    unsigned int size_per_elem = 4u;
    unsigned int total = count * size_per_elem; /* overflow */

    char *p = my_alloc((int)total);
    int i;

    if (!p) {
        /* fallback to some region anyway */
        p = heap_area;
    }

    for (i = 0; i < 64; i++) {
        p[i] = (char)i; /* may overflow intended region */
    }

    sink = p[0];
}
