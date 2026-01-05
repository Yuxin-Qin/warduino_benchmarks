#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;

    for (int i = -64; i < 64; i++) {
        unsigned char *p = buf + i;   /* for i<0, p < heap_base → underflow */
        *p = (unsigned char)(i & 0xff);
    }

    print_int(buf[0]);
}
