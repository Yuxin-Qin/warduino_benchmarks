
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Two buffers: small dst, large src. */
    unsigned long dst_size = 256;
    unsigned long src_size = 1024;

    unsigned long dst_off = heap_len / 8;
    unsigned long src_off = dst_off + dst_size;  /* src right after dst */

    unsigned char *dst = heap + dst_off;
    unsigned char *src = heap + src_off;

    /* Initialise src. */
    for (unsigned long i = 0; i < src_size; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    /* Bug: copy using src_size instead of dst_size. */
    for (unsigned long i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* writes past dst, potentially off end of heap */
    }

    sink = dst[0];
}

