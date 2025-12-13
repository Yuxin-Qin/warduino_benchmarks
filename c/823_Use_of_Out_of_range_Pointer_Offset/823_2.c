/* 823_2.c – CWE-823: off-by-one page offset beyond heap region */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Compute pointer just past the end of linear memory. */
    unsigned char *end_of_memory =
        heap_base + num_pages * WASM_PAGE_SIZE;

    /* Off-by-one page: one extra page beyond legal region. */
    unsigned char *bad_byte_ptr = end_of_memory + WASM_PAGE_SIZE;
    volatile int *bad = (int *)bad_byte_ptr;

    *bad = 0x82300002;  /* Out-of-range write */

    /* Touch heap_base to keep it live. */
    heap_base[0] = 2;
    print_int((int)heap_base[0]);
}
