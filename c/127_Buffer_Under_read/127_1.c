#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf = heap + 128;

    for (int i = -64; i < 0; i++) {
        unsigned char *p = buf + i;  /* reads before start of buffer, may cross below heap */
        sink = *p;
    }
}
