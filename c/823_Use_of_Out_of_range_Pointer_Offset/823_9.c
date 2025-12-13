/* 823_9.c – CWE-823: header/payload layout miscomputed using pages */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

struct Block {
    int header[4];
    int payload[4];
};

static struct Block block = { {0}, {0} };

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Correct payload base would be &block.payload[0]. */
    volatile int *base = &block.payload[0];

    /* Miscompute out-of-range offset using page size as element count. */
    int wrong_offset = WASM_PAGE_SIZE * num_pages + 4;
    volatile int *bad = base + wrong_offset;

    *bad = 0x82300009;

    block.header[0] = 9;
    print_int(block.header[0]);
}
