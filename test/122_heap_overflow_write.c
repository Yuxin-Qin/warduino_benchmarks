#define WASM_PAGE_SIZE 0x10000   /* 64KB, from WARDuino.h */

/* The linker provides the start of the heap */
extern unsigned char __heap_base[];

// @entrypoint
int start(void) {
  /* Number of pages in linear memory (does not touch linear memory) */
  int num_pages = __builtin_wasm_memory_size(0);

  /* Treat everything from __heap_base up to the end of linear memory as the heap */
  unsigned char *heap_base = __heap_base;

  /* Compute the first address *past* the end of linear memory.
     Linear memory is [0, num_pages * WASM_PAGE_SIZE).
     __heap_base is somewhere inside that region.
     This pointer arithmetic stays in registers; it does not read memory. */
  unsigned char *heap_oob =
      (unsigned char *)0 + (unsigned long)num_pages * WASM_PAGE_SIZE;

  /* Optional: do a few in-bounds heap writes, still write-only */
  for (int i = 0; i < 16; i++) {
    heap_base[i] = (unsigned char)i;   /* in-bounds writes */
  }

  /* This is the key: a write 1 byte beyond the end of linear memory.
     In Wasm this compiles to an out-of-bounds i32.store8 and should trap.
     Under CHERI this should raise a capability violation. */
  *heap_oob = 0xAA;

  return 0;
}
