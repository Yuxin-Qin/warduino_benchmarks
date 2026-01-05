#include "wasm_layout.h"

/*
 * CWE-823:
 * Compute pointer beyond entire heap and dereference once.
 */
static volatile unsigned char sink;

void start(void) {
    unsigned char *heap     = wasm_heap_base();
    int            pages    = wasm_pages();
    unsigned long  heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *base = heap + heap_len / 2;
    long           offset = (long)heap_len;

    unsigned char *p = base + offset;  /* out-of-range offset */

    sink = *p;
    print_int(pages);
}
