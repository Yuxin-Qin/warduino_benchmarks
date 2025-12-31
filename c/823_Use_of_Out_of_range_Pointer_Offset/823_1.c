#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();

    /* Treat full linear memory as one big buffer. */
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Choose a base somewhere in the middle of heap. */
    unsigned char *base = heap + (heap_len / 2);

    /*
     * Out-of-range pointer offset: step by +heap_len, which
     * carries the pointer beyond the end of linear memory.
     * On a CHERI host, this should breach capability bounds;
     * on a non-CHERI wasm host, it is an out-of-linear-memory
     * access (trap / segfault).
     */
    unsigned long offset = heap_len;
    unsigned char *p = base + offset;

    sink = *p;  /* Use of out-of-range pointer offset. */
}

