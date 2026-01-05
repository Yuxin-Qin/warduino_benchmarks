#include "wasm_layout.h"
extern void print_int(int);

/* Buffer logically starts in the middle of heap, but we step backwards
   beyond heap base. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    int sum = 0;

    for (int i = 0; i < 512; i++) {
        unsigned char *p = buf - (unsigned long)i;
        sum += *p;  /* for large i, p < heap base */
    }

    print_int(sum);
}
