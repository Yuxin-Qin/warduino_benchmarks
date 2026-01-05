#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

static int table[32];

void start(void) {
    int pages = __builtin_wasm_memory_size(0);
    for (int i = 0; i < 32; i++) {
        table[i] = i;
    }

    /* Use pages directly as index, far beyond [0..31]. */
    table[pages] = 0x12900001;

    print_string("129_1 done\n", 11);
}
