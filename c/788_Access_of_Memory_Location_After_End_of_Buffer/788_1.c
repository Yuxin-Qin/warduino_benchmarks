
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Sub-buffer in middle of heap. */
    unsigned long offset = heap_len / 3;
    unsigned long size   = 128;

    unsigned char *buf = heap + offset;

    /* Correctly write within the buffer. */
    for (unsigned long i = 0; i < size; i++) {
        buf[i] = (unsigned char)i;
    }

    /* Later buggy read: assumes size 256. */
    unsigned char sum = 0;
    for (unsigned long i = 0; i < 256; i++) {
        sum ^= buf[i];
    }

    sink = sum;
}

