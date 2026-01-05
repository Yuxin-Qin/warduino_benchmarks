#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *dst = heap + (heap_len / 2);
    unsigned char *src = heap;

    /* Copy beyond end of linear memory. */
    unsigned long len = (heap_len / 2) + 32;

    for (unsigned long i = 0; i < len; i++) {
        dst[i] = src[i];
    }

    print_string("120_1 done\n", 11);
}
