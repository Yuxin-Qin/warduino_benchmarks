#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src  = heap;
    unsigned char *dest = heap + heap_len / 2;

    unsigned long src_len = heap_len;
    for (unsigned long i = 0; i < src_len; i++) {
        dest[i] = src[i];  /* dest runs off end of linear memory */
    }

    print_int(dest[0]);
}
