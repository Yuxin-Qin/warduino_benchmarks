/* 823_3.c – CWE-823: Use of Out-of-range Pointer Offset */

#define WASM_PAGE_SIZE 0x10000  /* 64 KiB, WebAssembly page size */

extern unsigned char __heap_base[];  /* Provided by the Wasm toolchain */
extern void print_int(int);          /* WARDuino primitive */

/*
 * Bug: we treat the whole linear memory as one big buffer, then
 * use pointer arithmetic to walk *beyond* its end and dereference.
 */
void start(void) {
    unsigned char *heap = __heap_base;

    /* How many pages of linear memory exist right now? */
    int pages = __builtin_wasm_memory_size(0);

    /* Total bytes in linear memory. */
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Pick a base inside the heap (middle). */
    unsigned char *base = heap + heap_len / 2;

    /*
     * Compute an out-of-range offset that moves 'base' beyond
     * the end of linear memory.
     */
    unsigned long offset = heap_len;   /* one full heap-length forward */
    unsigned char *p = base + offset;  /* >>> out-of-range pointer offset <<< */

    /* CWE-823: dereference a pointer derived via an out-of-range offset. */
    *p = 0x7F;     /* write outside the allowed region */
    print_int(*p); /* may read back garbage or trigger a fault */
}
