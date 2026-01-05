#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + (heap_len / 2);

    unsigned long src_size = heap_len;       /* full linear memory */
    unsigned long dst_size = heap_len / 4;   /* smaller region */

    for (unsigned long i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* uses src size, overruns dst */
    }

    (void)dst_size;

    print_string("806_1 done\n", 11);
}
