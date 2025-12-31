#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

/* WebAssembly linear memory page size (64 KiB). */
#define WASM_PAGE_SIZE 0x10000

/* Exported by the wasm toolchain / runtime. */
extern unsigned char __heap_base[];

/* Return the start of the heap region. */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

/* Return number of wasm pages currently in linear memory 0. */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

#endif /* WASM_LAYOUT_H */
