#include "wasm_layout.h"
extern void print_int(int);

/* Writes before the beginning of the heap region. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    (void)wasm_pages(); /* unused in this pattern */

    unsigned char *buf = heap + 256;
    unsigned char *p = buf - 512;  /* definitely before heap base */

    for (int i = 0; i < 64; i++) {
        p[i] = (unsigned char)(i & 0xff);  /* underflow into unmapped region */
    }

    print_int(buf[0]);
}
