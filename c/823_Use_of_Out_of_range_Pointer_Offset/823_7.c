/* 823_7.c – CWE-823: integer overflow in pointer offset calculation */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static unsigned char buffer[128];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    unsigned char *base = buffer;

    /* Compute an offset in 32-bit int with potential overflow. */
    int offset = num_pages * WASM_PAGE_SIZE;
    offset += 0x7FFFFFF0;

    /* Cast back to pointer: this is an out-of-range offset. */
    unsigned char *bad_byte_ptr = base + offset;
    volatile int *bad = (int *)bad_byte_ptr;

    *bad = 0x82300007;

    buffer[0] = 7;
    print_int((int)buffer[0]);
}
