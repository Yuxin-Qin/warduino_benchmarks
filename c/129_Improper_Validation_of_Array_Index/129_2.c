#include "wasm_layout.h"
extern void print_int(int);

/* Uses an index derived from heap size without clamping to array size. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Place array near the very end of heap. */
    unsigned long arr_size = 64;
    unsigned char *arr = heap + heap_len - arr_size;

    unsigned long idx = heap_len / 2;  /* nonsensical, far too large */

    /* "Validate" index incorrectly: only checks idx >= 0. */
    if ((long)idx >= 0) {
        arr[idx] = 0x29;  /* out-of-bounds write beyond linear memory */
    }

    print_int(arr[0]);
}
