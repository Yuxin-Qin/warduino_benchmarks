#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *dst = heap + heap_len / 2;
    unsigned char *src = heap;

    unsigned long copy_len = heap_len;  /* overflows beyond end of heap */
    for (unsigned long i = 0; i < copy_len; i++) {
        dst[i] = src[i];
    }

    print_int(dst[0]);
}
