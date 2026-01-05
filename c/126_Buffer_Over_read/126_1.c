#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + (heap_len / 4);
    unsigned long len  = (heap_len / 4) + 32;

    for (unsigned long i = 0; i < len; i++) {
        unsigned char v = buf[i];
        print_int((int)v);
    }

    print_string("126_1 done\n", 11);
}
