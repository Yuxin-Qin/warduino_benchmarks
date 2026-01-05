#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *heap_obj = heap + heap_len / 4;
    unsigned long obj_size = heap_len / 2;

    for (unsigned long i = 0; i < obj_size + 128; i++) {
        heap_obj[i] = (unsigned char)(i & 0xff);
    }

    print_int(heap_obj[0]);
}
