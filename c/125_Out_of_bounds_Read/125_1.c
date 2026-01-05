#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *p = heap + heap_len + 32;  /* clearly past end of linear mem */
    sink = *p;
}
