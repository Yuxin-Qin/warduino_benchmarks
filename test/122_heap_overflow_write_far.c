// heap_overflow_write_far.c
// A write-only micro-benchmark that performs a *far* out-of-bounds heap write
// by storing to an address well beyond the end of Wasm linear memory.
//
// Build (wasi-sdk example):
//   clang --target=wasm32-wasi -nostdlib -Wl,--no-entry -Wl,--export=start -Wl,--allow-undefined -O2 \
//         heap_overflow_write_far.c -o heap_overflow_write_far.wasm
//
// Notes:
// - No libc used.
// - No printing required.
// - The final store should trap in a Wasm engine (OOB linear memory),
//   and should raise a capability fault under CHERI if linear memory is bounded.

#include <stdint.h>

#define WASM_PAGE_SIZE 0x10000u  /* 64KB */

// The linker provides the start of the heap in many WASI/Wasm setups.
// (Even if you don't use it, keeping it matches your environment.)
extern unsigned char __heap_base[];

// @entrypoint
int start(void) {
  // Size of linear memory in pages.
  uint32_t num_pages = (uint32_t)__builtin_wasm_memory_size(0);
  uint32_t mem_bytes = num_pages * WASM_PAGE_SIZE;

  // Optional: in-bounds, write-only activity (keeps this a "heap-ish" benchmark).
  // This writes starting at __heap_base; assumes there is at least 256 bytes of heap space.
  volatile unsigned char *heap_base = (volatile unsigned char *)__heap_base;
  for (uint32_t i = 0; i < 256; i++) {
    heap_base[i] = (unsigned char)i;   // in-bounds writes
  }

  // ---- FAR out-of-bounds write ----
  // Choose how far beyond the end you want to write:
  //   +1 page  = 65536 bytes
  //   +16 pages = 1MB
  const uint32_t FAR = 16u * WASM_PAGE_SIZE;   // 1MB past the end

  // Compute an address *past* the end of linear memory, then add FAR.
  // We form the pointer via integer arithmetic to avoid C null-pointer arithmetic UB.
  uintptr_t oob_addr = (uintptr_t)mem_bytes + (uintptr_t)FAR;

  // This store is the key event: an out-of-bounds write.
  // In Wasm, the backend should emit an i32.store8 that traps.
  *(volatile unsigned char *)oob_addr = 0xAA;

  return 0;
}
