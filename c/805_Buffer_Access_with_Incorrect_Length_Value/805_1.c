#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + 64;
    unsigned long real_len = 128;
    unsigned long wrong_len = heap_len;  /* incorrect length */

    for (unsigned long i = 0; i < wrong_len; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    (void)real_len;

    print_string("805_1 done\n", 11);
}
