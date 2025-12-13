/* 823_8.c – CWE-823: loop gradually stepping pointer across boundary */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static int ring[8];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    for (int i = 0; i < 8; i++) {
        ring[i] = i;
    }

    volatile int *p = ring;

    /* Step pointer in a loop, but stride is much too big. */
    int step_words = (num_pages * (WASM_PAGE_SIZE / (int)sizeof(int))) / 2;

    for (int k = 0; k < 3; k++) {
        p = p + step_words;  /* eventually way outside ring[] */
    }

    /* Final out-of-range write. */
    *p = 0x82300008;

    /* Preserve one valid element. */
    print_int(ring[0]);
}
