#include "wasm_layout.h"
extern void print_int(int);

/* Reads past the end of linear memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *p = heap + heap_len + 32;  /* beyond linear memory */
    int v = p[0];  /* out-of-bounds read */

    print_int(v);
}
