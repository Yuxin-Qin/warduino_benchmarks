#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    for (unsigned long i = 0; i <= heap_len + 32; i++) {
        heap[i] = (unsigned char)(i & 0xff);  /* last writes beyond linear memory */
    }
}
