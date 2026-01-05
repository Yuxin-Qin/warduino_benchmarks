/* 119_2.c – CWE-119: Improper Restriction of Operations within the Bounds
 * of a Memory Buffer, using an address beyond Wasm linear memory.
 *
 * The program:
 *   - Queries the current Wasm memory size in pages.
 *   - Computes mem_bytes = pages * 64KB.
 *   - Treats that as a “valid” buffer limit, but then adds an extra offset.
 *   - Forms a pointer to address mem_bytes + 16, which is strictly past
 *     the end of linear memory [0 .. mem_bytes-1].
 *   - Dereferences that pointer once.
 *
 * On a correct Wasm engine, this must trap as an out-of-bounds access.
 * On CHERI+WARDuino (with a capability bounded to linear memory), the
 * dereference should also violate the capability bounds.
 */

#define WASM_PAGE_SIZE 0x10000   /* 64KB – Wasm page size */

extern void print_int(int);      /* provided by the host runtime */
volatile unsigned char sink;     /* keep the load "live" */

void start(void) {
    /* Number of pages in memory 0 */
    int pages = __builtin_wasm_memory_size(0);

    /* Total linear memory size in bytes */
    unsigned long mem_bytes = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Incorrectly assume we can safely go beyond mem_bytes */
    unsigned long bad_addr = mem_bytes + 16;   /* beyond [0 .. mem_bytes-1] */

    /* Fabricate a pointer to that out-of-bounds address */
    unsigned char *p = (unsigned char *)bad_addr;

    /* Single out-of-linear-memory access */
    sink = *p;

    /* Just to have some side effect if it ever "succeeds" (it should not) */
    print_int((int)pages);
}
