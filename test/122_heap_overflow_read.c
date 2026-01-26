#define WASM_PAGE_SIZE 0x10000   /* 64KB, from WARDuino.h */

extern unsigned char __heap_base[]; /* linker initializes this value */

int data[10];


// @entrypoint
int start() {
  int num_pages = __builtin_wasm_memory_size(0);
  unsigned char *heap_base = __heap_base;

  print_int(num_pages);
  print_int((int)heap_base);
  print_int((int)(heap_base[0]));
  heap_base +=(WASM_PAGE_SIZE*num_pages);
  heap_base--;
  print_int((int)(heap_base[0]));
  return 0;
}