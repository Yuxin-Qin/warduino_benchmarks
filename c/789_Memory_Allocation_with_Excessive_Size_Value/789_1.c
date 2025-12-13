
#include "wasm_layout.h"

volatile unsigned char sink;
static unsigned long alloc_offset = 0;

static unsigned char *my_alloc(unsigned long n) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Bug: allow n that pushes us just past heap end. */
    unsigned long max_safe = heap_len - alloc_offset;
    unsigned long requested = n;

    if (requested > max_safe + 32) {
        /* still accept too-large request */
        requested = max_safe + 32;
    }

    unsigned char *p = heap + alloc_offset;
    alloc_offset += requested;
    return p;
}

void start(void) {
    /* Ask for a huge block near end of memory. */
    unsigned char *p = my_alloc(0x7fffffff);

    /* Write a small region, but p itself may already be out of bounds. */
    for (int i = 0; i < 64; i++) {
        p[i] = (unsigned char)i;
    }

    sink = p[0];
}

