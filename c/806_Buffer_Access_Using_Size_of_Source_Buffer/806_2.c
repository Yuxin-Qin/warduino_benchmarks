#include "wasm_layout.h"
extern void print_int(int);

/* Uses size of a "source" range that crosses heap end to copy into a
   destination region, pushing writes out of linear memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap + heap_len - 64;
    unsigned char *dst = heap + heap_len / 2;

    unsigned long src_size = heap_len;  /* nonsensical */
    for (unsigned long i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* crosses end of heap */
    }

    print_int(dst[0]);
}
