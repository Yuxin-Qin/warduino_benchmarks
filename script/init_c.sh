#!/usr/bin/env bash
set -euo pipefail

# Root of repo = parent of this script
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_ROOT="${ROOT_DIR}/c"

mkdir -p "${SRC_ROOT}"

echo "Creating C benchmarks under: ${SRC_ROOT}"
echo

########################################
# Common header for page-aware tests
########################################
cat > "${SRC_ROOT}/wasm_layout.h" <<'EOF'
#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

/* WebAssembly linear memory page size (64 KiB). */
#define WASM_PAGE_SIZE 0x10000

/* Exported by the wasm toolchain / runtime. */
extern unsigned char __heap_base[];

/* Return the start of the heap region. */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

/* Return number of wasm pages currently in linear memory 0. */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

#endif /* WASM_LAYOUT_H */
EOF

########################################
# Helper to write a single C file
########################################
make_cwe_file() {
  local subdir="$1"
  local filename="$2"
  local content="$3"

  local dir="${SRC_ROOT}/${subdir}"
  mkdir -p "${dir}"
  local path="${dir}/${filename}.c"

  echo "  -> ${path}"
  printf '%s\n' "${content}" > "${path}"
}

########################################
# 119 – Improper Restriction of Operations within the Bounds of a Memory Buffer
########################################
make_cwe_file \
  "119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer" \
  "119_1" \
'volatile char sink;

void start(void) {
    char dest[16];
    char src[32];
    int i;

    for (i = 0; i < 32; i++) {
        src[i] = (char)i;
    }

    /* Improper restriction: copy full src into smaller dest. */
    for (i = 0; i < 32; i++) {
        dest[i] = src[i];   /* writes past dest[15] */
    }

    sink = dest[0];
}
'

########################################
# 120 – Buffer Copy without Checking Size of Input
########################################
make_cwe_file \
  "120_Buffer_Copy_without_Checking_Size_of_Input" \
  "120_1" \
'volatile char sink;

void start(void) {
    char dst[8];
    char src[64];
    int i;

    for (i = 0; i < 64; i++) {
        src[i] = (char)(i + 1);
    }

    /* No length check on input size. */
    for (i = 0; i < 64; i++) {
        dst[i] = src[i];    /* classic buffer overflow */
    }

    sink = dst[0];
}
'

########################################
# 121 – Stack-based Buffer Overflow
########################################
make_cwe_file \
  "121_Stack_based_Buffer_Overflow" \
  "121_1" \
'volatile int sink;

void start(void) {
    int stack_buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        stack_buf[i] = i;
    }

    /* Stack-based overflow: write beyond end of stack_buf. */
    for (i = 16; i < 32; i++) {
        stack_buf[i] = i * 2;   /* out-of-bounds on stack */
    }

    sink = stack_buf[0];
}
'

########################################
# 122 – Heap-based Buffer Overflow (simulated heap)
########################################
make_cwe_file \
  "122_Heap_based_Buffer_Overflow" \
  "122_1" \
'volatile int sink;
static int fake_heap[16];

static int *my_alloc(int n) {
    /* Very naive allocator returning base of fake_heap. */
    (void)n;
    return fake_heap;
}

void start(void) {
    int *p = my_alloc(16);
    int i;

    /* Correct writes. */
    for (i = 0; i < 16; i++) {
        p[i] = i;
    }

    /* Heap-based overflow: write far beyond allocated 16 ints. */
    for (i = 16; i < 40; i++) {
        p[i] = i * 3;
    }

    sink = fake_heap[0];
}
'

########################################
# 124 – Buffer Underwrite (Buffer Underflow)
########################################
make_cwe_file \
  "124_Buffer_Underwrite" \
  "124_1" \
'volatile int sink;

void start(void) {
    int buf[16];
    int *p = &buf[8];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = 0;
    }

    /* Underwrite: for i < 8, we write before buf[0]. */
    for (i = 0; i < 16; i++) {
        p[i - 8] = i;   /* buf[-8]..buf[7] on first half of loop */
    }

    sink = buf[8];
}
'

########################################
# 125 – Out-of-bounds Read
########################################
make_cwe_file \
  "125_Out_of_bounds_Read" \
  "125_1" \
'volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Out-of-bounds read well past end. */
    for (i = 0; i < 32; i++) {
        sink = buf[i];  /* reads undefined memory for i >= 16 */
    }
}
'

########################################
# 126 – Buffer Over-read
########################################
make_cwe_file \
  "126_Buffer_Over_read" \
  "126_1" \
'volatile int sink;

void start(void) {
    int src[16];
    int i;

    for (i = 0; i < 16; i++) {
        src[i] = i * 2;
    }

    /* Over-read: read beyond src length. */
    for (i = 0; i < 40; i++) {
        int v = src[i];   /* out-of-bounds for i >= 16 */
        sink = v;
    }
}
'

########################################
# 127 – Buffer Under-read
########################################
make_cwe_file \
  "127_Buffer_Under_read" \
  "127_1" \
'volatile int sink;

void start(void) {
    int buf[16];
    int *p = &buf[8];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Under-read: for i < 8, we read before buf[0]. */
    for (i = 0; i < 16; i++) {
        int v = p[i - 8];  /* buf[-8]..buf[7] */
        sink = v;
    }
}
'

########################################
# 129 – Improper Validation of Array Index
########################################
make_cwe_file \
  "129_Improper_Validation_of_Array_Index" \
  "129_1" \
'volatile int sink;

void start(void) {
    int table[10];
    int i;

    for (i = 0; i < 10; i++) {
        table[i] = i;
    }

    /* Unvalidated index far beyond array size. */
    int idx = 25;

    table[idx] = 999;  /* no bounds check at all */
    sink = table[0];
}
'

########################################
# 131 – Incorrect Calculation of Buffer Size
########################################
make_cwe_file \
  "131_Incorrect_Calculation_of_Buffer_Size" \
  "131_1" \
'volatile char sink;

void start(void) {
    int n = 8;
    char buf[8];
    int i;

    /* Incorrectly using element count as byte count while assuming int-size. */
    int bytes = n * (int)sizeof(int);  /* likely > 8 */

    for (i = 0; i < bytes; i++) {
        buf[i] = (char)i;   /* writes past buf[7] */
    }

    sink = buf[0];
}
'

########################################
# 786 – Access of Memory Location Before Start of Buffer
########################################
make_cwe_file \
  "786_Access_of_Memory_Location_Before_Start_of_Buffer" \
  "786_1" \
'volatile int sink;

void start(void) {
    int buf[16];
    int *p = &buf[4];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Access before start of buffer using negative index. */
    for (i = -4; i < 4; i++) {
        int v = p[i];   /* for i < 0, p[i] is before buf[0] */
        sink = v;
    }
}
'

########################################
# 787 – Out-of-bounds Write
########################################
make_cwe_file \
  "787_Out_of_bounds_Write" \
  "787_1" \
'volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = 0;
    }

    /* Out-of-bounds write beyond end of array. */
    for (i = 0; i < 40; i++) {
        buf[i] = i;    /* writes out of bounds when i >= 16 */
    }

    sink = buf[0];
}
'

########################################
# 788 – Access of Memory Location After End of Buffer
########################################
make_cwe_file \
  "788_Access_of_Memory_Location_After_End_of_Buffer" \
  "788_1" \
'volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Access well after end of buffer. */
    for (i = 16; i < 32; i++) {
        int v = buf[i];  /* beyond buf[15] */
        sink = v;
    }
}
'

########################################
# 805 – Buffer Access with Incorrect Length Value
########################################
make_cwe_file \
  "805_Buffer_Access_with_Incorrect_Length_Value" \
  "805_1" \
'volatile int sink;

void start(void) {
    int buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = i;
    }

    /* Incorrect length (too large) used for iteration. */
    int length = 64;

    for (i = 0; i < length; i++) {
        int v = buf[i];   /* out-of-bounds for i >= 16 */
        sink = v;
    }
}
'

########################################
# 806 – Buffer Access Using Size of Source Buffer
########################################
make_cwe_file \
  "806_Buffer_Access_Using_Size_of_Source_Buffer" \
  "806_1" \
'volatile int sink;

void start(void) {
    int src[32];
    int dst[8];
    int i;

    for (i = 0; i < 32; i++) {
        src[i] = i;
    }

    /* Uses size of source array to index destination. */
    int src_size = 32;

    for (i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* out-of-bounds for i >= 8 */
    }

    sink = dst[0];
}
'

########################################
# 823 – Use of Out-of-range Pointer Offset (page-aware)
########################################
make_cwe_file \
  "823_Use_of_Out_of_range_Pointer_Offset" \
  "823_1" \
'#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();

    /* Treat full linear memory as one big buffer. */
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Choose a base somewhere in the middle of heap. */
    unsigned char *base = heap + (heap_len / 2);

    /*
     * Out-of-range pointer offset: step by +heap_len, which
     * carries the pointer beyond the end of linear memory.
     * On a CHERI host, this should breach capability bounds;
     * on a non-CHERI wasm host, it is an out-of-linear-memory
     * access (trap / segfault).
     */
    unsigned long offset = heap_len;
    unsigned char *p = base + offset;

    sink = *p;  /* Use of out-of-range pointer offset. */
}
'

echo
echo "Done. C benchmarks and wasm_layout.h created under: ${SRC_ROOT}"
