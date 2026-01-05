#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Treat top quarter of heap as "stack frame". */
    unsigned char *frame = heap + (heap_len * 3) / 4;
    unsigned long frame_size = heap_len / 8;

    /* Overflow frame upward past end of memory. */
    for (unsigned long i = 0; i < frame_size + 64; i++) {
        frame[i] = (unsigned char)(i & 0xff);
    }

    print_string("121_1 done\n", 11);
}
