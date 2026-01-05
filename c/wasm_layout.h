#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

/* 64 KiB Wasm page size */
#define WASM_PAGE_SIZE 0x10000u

/* Provided by linker in WARDuino */
extern unsigned char __heap_base[];

/* Start of dynamic region used as heap */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

/* Current number of pages in memory(0) */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

#endif
