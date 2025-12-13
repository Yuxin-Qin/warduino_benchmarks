/* 823_4.c – CWE-823: negative offset stepping *before* heap_base */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Start from heap_base + some small positive offset. */
    unsigned char *p = heap_base + 64;

    /* Large negative offset: walk backwards across multiple pages. */
    int negative_bytes = num_pages * WASM_PAGE_SIZE + 256;
    unsigned char *bad_byte_ptr = p - negative_bytes;
    volatile int *bad = (int *)bad_byte_ptr;

    *bad = 0x82300004;  /* Out-of-range write before region */

    heap_base[1] = 4;
    print_int((int)heap_base[1]);
}
