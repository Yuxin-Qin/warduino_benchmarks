#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap;
    int idx = (int)heap_len;  /* use page-derived value as index */

    buf[idx] = 0x29;  /* well past linear memory */
    print_int(buf[0]);
}
