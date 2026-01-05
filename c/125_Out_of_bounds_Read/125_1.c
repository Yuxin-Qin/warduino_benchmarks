#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    for (unsigned long i = 0; i < heap_len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    /* Read past end of linear memory. */
    unsigned char v = heap[heap_len + 8];
    print_int((int)v);

    print_string("125_1 done\n", 11);
}
