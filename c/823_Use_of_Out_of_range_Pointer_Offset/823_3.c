/* 823_3.c – CWE-823: wrong struct field chosen as base + huge offset */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

struct Region {
    int header;
    int payload[8];
};

static struct Region region = { 0, { 0 } };

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Correct payload base would be &region.payload[0]. */
    volatile int *base = &region.header;  /* Wrong base (header) */

    /* Out-of-range offset scaled by pages. */
    int offset_words = (num_pages * (WASM_PAGE_SIZE / (int)sizeof(int))) + 32;
    volatile int *bad = base + offset_words;

    *bad = 0x82300003;       /* Out-of-range pointer offset use */

    region.payload[0] = 3;   /* Keep struct alive */
    print_int(region.payload[0]);
}
