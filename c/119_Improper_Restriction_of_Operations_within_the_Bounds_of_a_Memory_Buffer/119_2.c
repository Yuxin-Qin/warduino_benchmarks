#include "wasm_layout.h"
extern void print_int(int);

/* Copies heap_len bytes from the start of heap into its middle,
   overrunning linear memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + heap_len / 2;
    unsigned long len = heap_len;  /* incorrectly allowed */

    for (unsigned long i = 0; i < len; i++) {
        dst[i] = src[i];  /* crosses end of linear memory */
    }

    print_int(dst[0]);
}
