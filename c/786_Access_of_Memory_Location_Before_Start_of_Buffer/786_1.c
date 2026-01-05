#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf = heap + 256;

    for (int i = -512; i < 0; i++) {
        unsigned char *p = buf + i;  /* can go below linear memory start */
        sink = *p;
    }
}
