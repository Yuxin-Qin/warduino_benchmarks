#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned long offset = 256;

    unsigned char *buf = heap + offset;

    for (int i = -512; i < 0; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    print_string("786_1 done\n", 11);
}
