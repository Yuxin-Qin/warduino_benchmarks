#include "wasm_layout.h"
extern void print_int(int);

/* Emulates a stack at the end of linear memory and pushes past it. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long stack_size = 256;
    unsigned char *stack_base = heap + heap_len - stack_size;
    unsigned char *sp = stack_base;

    /* Missing stack-limit check: pushes way beyond heap end. */
    for (unsigned long i = 0; i < heap_len; i++) {
        *sp++ = (unsigned char)(i & 0xff);
    }

    print_int(stack_base[0]);
}
