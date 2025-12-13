
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* buffer is last 1 KiB of heap */
    unsigned long buf_size = 1024;
    unsigned char *buf = heap + heap_len - buf_size;

    /* Incorrect loop bound: writes buf_size + 64 bytes. */
    for (unsigned long i = 0; i < buf_size + 64; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    sink = buf[0];
}

