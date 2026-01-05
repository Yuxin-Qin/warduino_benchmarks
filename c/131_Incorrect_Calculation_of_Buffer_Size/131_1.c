#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);

    /* Wrong size: assume only 1 page regardless of actual. */
    unsigned long wrong_len = WASM_PAGE_SIZE;

    /* But still loop up to pages*page_size, possibly past real memory. */
    unsigned long real_len = (unsigned long)pages * WASM_PAGE_SIZE;

    for (unsigned long i = 0; i < real_len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    (void)wrong_len;

    print_string("131_1 done\n", 11);
}
