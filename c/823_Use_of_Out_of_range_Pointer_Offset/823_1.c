/* 823_1.c – CWE-823: simple huge forward offset from heap_base */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    /* Diagnostics: how big is memory, where is heap base. */
    print_int(num_pages);
    print_int((int)heap_base);

    /* Base pointer into heap, then a huge forward offset in bytes. */
    volatile int *p = (int *)heap_base;

    /* Out-of-range pointer offset: stride crosses all pages and more. */
    int byte_offset = num_pages * WASM_PAGE_SIZE + 128;
    volatile int *bad = (int *)((unsigned char *)p + byte_offset);

    /* Misuse: write through an out-of-range pointer offset. */
    *bad = 0x82300001;

    /* Keep p “live” so compiler does not optimise it away. */
    *p = 1;
    print_int(*p);
}
