#include "wasm_layout.h"

/*
 * CWE-119:
 * Treat the entire linear memory as one big "log buffer" and
 * perform a bulk copy of all bytes into the second half,
 * without restricting the operation to the remaining buffer size.
 */
void start(void) {
    unsigned char *heap     = wasm_heap_base();
    int            pages    = wasm_pages();
    unsigned long  heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + heap_len / 2;  /* only half of heap available */

    /* Caller requests a copy of the entire heap into the second half. */
    unsigned long requested = heap_len;

    /* Improper restriction: we never clamp requested to the dst capacity. */
    unsigned long copy_len = requested;

    for (unsigned long i = 0; i < copy_len; i++) {
        dst[i] = src[i];  /* out-of-bounds write once i >= heap_len/2 */
    }

    print_int(dst[0]);
}
