#include "wasm_layout.h"
extern void print_int(int);

/* Passes total heap_len instead of logical length for a sub-buffer. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    unsigned long logical_len = 128;
    unsigned long length_param = heap_len;  /* wrong */

    for (unsigned long i = 0; i < length_param; i++) {
        buf[i] = (unsigned char)(i & 0xff);  /* out of heap range */
    }

    print_int(buf[0]);
}
