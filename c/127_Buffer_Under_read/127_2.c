#include "wasm_layout.h"
extern void print_int(int);

/* Reads before the start of a heap buffer, below heap base. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    (void)wasm_pages();

    unsigned char *buf = heap + 256;
    int sum = 0;

    for (int i = -64; i < 64; i++) {
        unsigned char *p = buf + i;
        sum += *p;   /* for i<0, dereference < heap base */
    }

    print_int(sum);
}
