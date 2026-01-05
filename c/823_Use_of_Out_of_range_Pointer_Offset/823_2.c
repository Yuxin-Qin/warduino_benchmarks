#include "wasm_layout.h"
extern void print_int(int);

/* Builds a pointer by adding a huge offset to a base inside heap. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *base = heap + heap_len / 2;
    unsigned long offset = heap_len;  /* out-of-range offset */

    unsigned char *p = base + offset;  /* definitely beyond linear memory */
    int v = *p;  /* out-of-range pointer dereference */

    print_int(v);
}
