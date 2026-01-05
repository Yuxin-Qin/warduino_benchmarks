#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);

    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;
    unsigned long buf_len  = heap_len + 256;  /* miscomputed size */

    for (unsigned long i = 0; i < buf_len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    print_int(heap[0]);
}
