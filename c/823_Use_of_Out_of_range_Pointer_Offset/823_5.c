/* 823_5.c – CWE-823: using page count as an array index directly */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static int table[32];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Fill table with something deterministic. */
    for (int i = 0; i < 32; i++) {
        table[i] = i;
    }

    /* Misuse: treat num_pages as a valid table index. */
    int idx = num_pages;  /* Typically far beyond [0..31] */
    volatile int *bad = &table[idx];

    *bad = 0x82300005;  /* Out-of-range pointer offset use */

    /* Keep a valid entry alive. */
    print_int(table[0]);
}
