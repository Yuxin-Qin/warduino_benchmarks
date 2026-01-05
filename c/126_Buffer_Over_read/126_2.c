#include "wasm_layout.h"
extern void print_int(int);

/* "Scan" past a logical buffer end into unmapped memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    unsigned long logical_len = 64;
    unsigned long scan_len = heap_len;  /* badly larger */

    int sum = 0;
    for (unsigned long i = 0; i < scan_len; i++) {
        sum += buf[i];  /* crosses end of linear memory */
    }

    print_int(sum);
}
