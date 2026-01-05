#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

static unsigned char *fake_heap = 0;
static unsigned long fake_size  = 0;

static unsigned char *my_malloc(unsigned long n) {
    if (!fake_heap) {
        fake_heap = __heap_base;
        int pages = __builtin_wasm_memory_size(0);
        fake_size = (unsigned long)pages * WASM_PAGE_SIZE;
    }
    return fake_heap; /* one big region */
}

void start(void) {
    unsigned char *buf = my_malloc(64);
    unsigned long n = fake_size + 32; /* overflow beyond allocated region */

    for (unsigned long i = 0; i < n; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    print_string("122_1 done\n", 11);
}
