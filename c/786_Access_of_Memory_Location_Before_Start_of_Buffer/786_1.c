
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Define a buffer window that starts well inside heap. */
    unsigned long offset = heap_len / 2;
    unsigned long size   = 256;

    unsigned char *buf = heap + offset;

    /* Indexing uses signed index and allows negative values. */
    for (int i = -64; i < (int)size; i++) {
        unsigned char *p = buf + i;  /* for i < 0, p < buf */
        *p = (unsigned char)(i & 0xff);
    }

    sink = buf[0];
}

