
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long buf_offset = heap_len / 4;
    unsigned long buf_size   = 512;
    unsigned char *buf = heap + buf_offset;

    /* Actual number of elements in buf is buf_size. */
    unsigned long actual_len = buf_size;

    /* Incorrect length used in loop (off by +256). */
    unsigned long used_len = actual_len + 256;

    for (unsigned long i = 0; i < used_len; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    sink = buf[0];
}

