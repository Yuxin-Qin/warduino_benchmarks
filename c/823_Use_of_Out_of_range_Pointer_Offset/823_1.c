
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Base inside heap. */
    unsigned char *base = heap + heap_len / 2;

    /* Compute an out-of-range offset deliberately beyond heap end. */
    long offset = (long)heap_len;   /* moves pointer beyond heap upper bound */

    unsigned char *p = base + offset;  /* out-of-range pointer offset */

    /* Single dereference; on CHERI this should fault, on non-CHERI it is UB. */
    sink = *p;
}

