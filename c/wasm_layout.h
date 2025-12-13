/* wasm_layout.h
 *
 * Common helpers for page-aware WebAssembly benchmarks.
 * No standard library is used.
 */

#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

/* WebAssembly linear memory page size (fixed by spec). */
#define WASM_PAGE_SIZE 0x10000u

/* Provided by wasm-ld / runtime: start of the heap in linear memory. */
extern unsigned char __heap_base[];

/* Return number of pages in memory 0 at runtime. */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

/* Heap base pointer. */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

#endif /* WASM_LAYOUT_H */
