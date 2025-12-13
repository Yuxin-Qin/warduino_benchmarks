
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Point near end of heap, then read beyond. */
    unsigned char *p = heap + heap_len - 8;

    /* Read 32 bytes, last 24 bytes are out-of-bounds. */
    unsigned char sum = 0;
    for (int i = 0; i < 32; i++) {
        sum ^= p[i];
    }

    sink = sum;
}

