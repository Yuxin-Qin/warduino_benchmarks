#include "wasm_layout.h"

static unsigned char *heap_ptr = 0;

static void init_heap(void) {
    if (!heap_ptr) {
        heap_ptr = wasm_heap_base();
    }
}

static void *my_alloc(unsigned long bytes) {
    init_heap();
    unsigned char *p = heap_ptr;
    heap_ptr += bytes;  /* no heap bound check */
    return (void *)p;
}

/*
 * CWE-122:
 * Allocate a message buffer but copy more bytes than allocated.
 */
void start(void) {
    unsigned char *src;
    unsigned char *dst;
    unsigned long  alloc_size = 32;
    unsigned long  to_copy    = 64;
    int            sum        = 0;

    src = (unsigned char *)my_alloc(64);
    dst = (unsigned char *)my_alloc(alloc_size);

    for (unsigned long i = 0; i < 64; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    /* Overflow dst. */
    for (unsigned long i = 0; i < to_copy; i++) {
        dst[i] = src[i];
    }

    for (unsigned long i = 0; i < alloc_size; i++) {
        sum += dst[i];
    }

    print_int(sum);
}
