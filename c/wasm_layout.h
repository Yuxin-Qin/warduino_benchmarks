#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

/* Start of linear heap as seen by C code compiled to Wasm. */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

/* Number of 64 KiB pages in the default Wasm memory. */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

#endif /* WASM_LAYOUT_H */
