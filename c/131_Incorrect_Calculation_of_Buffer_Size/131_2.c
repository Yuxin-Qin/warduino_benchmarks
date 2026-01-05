#include "wasm_layout.h"
extern void print_int(int);

/* Treats heap as array of 8-byte items but uses char indexing. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long items = heap_len / 8;   /* "element count" */
    unsigned long bytes_to_write = items * 8 + 64;  /* off by +64 */

    for (unsigned long i = 0; i < bytes_to_write; i++) {
        heap[i] = (unsigned char)(i & 0xff);  /* overruns end of heap */
    }

    print_int(heap[0]);
}
