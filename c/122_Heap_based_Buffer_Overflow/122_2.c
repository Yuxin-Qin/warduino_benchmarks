#include "wasm_layout.h"
extern void print_int(int);

/* Simple bump "allocator" that ignores remaining heap and overruns it. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *heap_end = heap + heap_len;
    unsigned char *p = heap_end - 128;  /* pretend last allocation */

    unsigned long requested = 512;      /* too big for remaining heap */
    for (unsigned long i = 0; i < requested; i++) {
        p[i] = (unsigned char)(i & 0xff);  /* crosses linear memory end */
    }

    print_int(p[0]);
}
