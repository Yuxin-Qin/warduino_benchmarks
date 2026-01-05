#include "wasm_layout.h"
extern void print_int(int);

/* Single write exactly at heap_len and beyond. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    heap[heap_len] = 0x7;   /* write one byte beyond linear memory */

    print_int((int)heap[0]);
}
