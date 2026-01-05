#include "wasm_layout.h"
extern void print_int(int);

/* Reads just past the end of heap. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *past_end = heap + heap_len;
    int v = past_end[1];  /* one byte after heap end */

    print_int(v);
}
