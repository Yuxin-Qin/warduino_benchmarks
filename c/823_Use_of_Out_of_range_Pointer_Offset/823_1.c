#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Base inside heap. */
    unsigned char *base = heap + heap_len / 2;

    /* Offset that walks beyond linear memory. */
    long offset = (long)heap_len;

    unsigned char *p = base + offset;  /* out-of-range pointer offset */

    sink = *p;

    print_string("823_1 done\n", 11);
}
