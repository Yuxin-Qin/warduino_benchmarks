
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Define a sub-buffer inside the heap. */
    unsigned long offset = heap_len / 4;
    unsigned long size   = heap_len / 8;

    unsigned char *buf = heap + offset;      /* buffer [buf, buf+size) */

    /* Underwrite: step before buf by 32 bytes. */
    unsigned char *under = buf - 32;

    for (unsigned long i = 0; i < 64; i++) {
        under[i] = (unsigned char)i;         /* may fall below heap base */
    }

    sink = under[0];
}

