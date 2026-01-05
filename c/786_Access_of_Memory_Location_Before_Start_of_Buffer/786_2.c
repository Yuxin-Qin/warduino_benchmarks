#include "wasm_layout.h"

/*
 * CWE-786:
 * Window in middle of heap; loop uses negative index.
 */
void start(void) {
    unsigned char *heap     = wasm_heap_base();
    int            pages    = wasm_pages();
    unsigned long  heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long  offset = heap_len / 2;
    unsigned char *window = heap + offset;
    int            sum    = 0;

    for (int i = 0; i < 256; i++) {
        window[i] = (unsigned char)(i & 0xff);
    }

    for (int i = -32; i < 64; i++) {
        unsigned char *p = window + i;  /* may underflow window */
        sum += *p;
    }

    print_int(sum);
}
