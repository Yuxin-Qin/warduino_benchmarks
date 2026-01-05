#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf  = heap + 64;

    for (int i = -64; i < 64; i++) {
        unsigned char *p = buf + i;
        unsigned char v  = *p;
        print_int((int)v);
    }

    print_string("127_1 done\n", 11);
}
