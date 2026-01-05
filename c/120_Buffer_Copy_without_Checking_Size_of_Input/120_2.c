#include "wasm_layout.h"
extern void print_int(int);

static void copy_unchecked(unsigned char *dst,
                           const unsigned char *src,
                           unsigned long len) {
    for (unsigned long i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + heap_len / 2;

    /* "User length" not checked against buffer size. */
    unsigned long user_len = heap_len + 32;  /* definitely past end */

    copy_unchecked(dst, src, user_len);

    print_int(dst[0]);
}
