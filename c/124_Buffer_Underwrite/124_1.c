#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf  = heap + 128;

    /* Write before start of buffer; if heap == linear start this may
       also cross below linear memory on small configs. */
    for (int i = -256; i < 0; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    print_string("124_1 done\n", 11);
}
