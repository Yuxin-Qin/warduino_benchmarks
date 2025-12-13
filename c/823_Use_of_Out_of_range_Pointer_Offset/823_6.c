/* 823_6.c – CWE-823: mixing byte and word counts in offset computation */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static int g_words[64];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    volatile int *base = g_words;

    /* Compute stride as if WASM_PAGE_SIZE already counted words. */
    int stride_words = num_pages * WASM_PAGE_SIZE;  /* actually bytes */

    /* base + stride_words now leaps far beyond g_words. */
    volatile int *bad = base + stride_words;

    *bad = 0x82300006;  /* Out-of-range write */

    g_words[1] = 6;
    print_int(g_words[1]);
}
