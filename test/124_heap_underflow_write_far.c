// heap_underflow_write.c
#define WASM_PAGE_SIZE 0x10000   /* 64KB, from WARDuino.h */

extern unsigned char __heap_base[];
extern void print_int(int);

// @entrypoint
int start() {
  int num_pages = __builtin_wasm_memory_size(0);
  unsigned char *heap_base = __heap_base;

  // Address one byte *before* heap
  unsigned char *bad = heap_base - (16 * WASM_PAGE_SIZE);  // 1MB underflow //-1 for short underflow

    // *** Heap underflow WRITE: single out-of-bounds store ***
  *bad = 0x42;   // Only write, no read through 'bad'

  // Context prints (all in-bounds)
  print_int(num_pages);
  print_int((int)heap_base);

  return 0;
}
