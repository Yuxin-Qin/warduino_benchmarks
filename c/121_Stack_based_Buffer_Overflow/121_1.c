#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *stack_frame = heap + heap_len - 32;

    for (int i = 0; i < 64; i++) {
        stack_frame[i] = (unsigned char)i;  /* writes past end of linear memory */
    }

    print_int(stack_frame[0]);
}
