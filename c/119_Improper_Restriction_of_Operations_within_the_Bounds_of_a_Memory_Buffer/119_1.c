
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();

    /* Treat entire heap as one big buffer of length heap_len. */
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Incorrect sanitised length: off by +16 beyond heap. */
    unsigned long len = heap_len + 16;

    /* Copy loop uses len as if it were safe. */
    for (unsigned long i = 0; i < len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    sink = heap[0];
}

