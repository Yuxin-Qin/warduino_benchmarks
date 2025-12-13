/* 823_10.c – CWE-823: pointer walks from heap into unrelated globals */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static int globals_a[4];
static int globals_b[4];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    for (int i = 0; i < 4; i++) {
        globals_a[i] = i;
        globals_b[i] = 100 + i;
    }

    /* Start in the heap region. */
    volatile int *base = (int *)heap_base;

    /*
     * Misuse: assume globals are “one page after” heap_base.
     * This fabricates a cross-region pointer that is out-of-range
     * relative to the capability region.
     */
    volatile int *bad = (int *)(heap_base + WASM_PAGE_SIZE * num_pages);
    bad += 8;  /* additional word offset */

    *bad = 0x82300010;

    print_int(globals_a[0]);
    print_int(globals_b[0]);
}
