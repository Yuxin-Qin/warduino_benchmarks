#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    unsigned long len = heap_len / 2 + 64;

    for (unsigned long i = 0; i < len; i++) {
        sink = buf[i];
    }
}
