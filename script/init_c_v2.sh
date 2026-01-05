#!/usr/bin/env bash
set -euo pipefail

# Repo root = parent of this script
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_ROOT="${ROOT_DIR}/c"

mkdir -p "${SRC_ROOT}"

############################################
# Common header: wasm_layout.h
############################################
cat > "${SRC_ROOT}/wasm_layout.h" <<'EOF'
#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

/* 64 KiB Wasm page size */
#define WASM_PAGE_SIZE 0x10000u

/* Provided by linker in WARDuino */
extern unsigned char __heap_base[];

/* Start of dynamic region used as heap */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

/* Current number of pages in memory(0) */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

#endif
EOF

############################################
# 119 – Improper Restriction of Operations
############################################
D119_DIR="${SRC_ROOT}/119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer"
mkdir -p "${D119_DIR}"
cat > "${D119_DIR}/119_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Copies heap_len bytes from the start of heap into its middle,
   overrunning linear memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + heap_len / 2;
    unsigned long len = heap_len;  /* incorrectly allowed */

    for (unsigned long i = 0; i < len; i++) {
        dst[i] = src[i];  /* crosses end of linear memory */
    }

    print_int(dst[0]);
}
EOF

############################################
# 120 – Buffer Copy without Checking Size
############################################
D120_DIR="${SRC_ROOT}/120_Buffer_Copy_without_Checking_Size_of_Input"
mkdir -p "${D120_DIR}"
cat > "${D120_DIR}/120_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

static void copy_unchecked(unsigned char *dst,
                           const unsigned char *src,
                           unsigned long len) {
    for (unsigned long i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + heap_len / 2;

    /* "User length" not checked against buffer size. */
    unsigned long user_len = heap_len + 32;  /* definitely past end */

    copy_unchecked(dst, src, user_len);

    print_int(dst[0]);
}
EOF

############################################
# 121 – Stack-based Buffer Overflow
############################################
D121_DIR="${SRC_ROOT}/121_Stack_based_Buffer_Overflow"
mkdir -p "${D121_DIR}"
cat > "${D121_DIR}/121_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Emulates a stack at the end of linear memory and pushes past it. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long stack_size = 256;
    unsigned char *stack_base = heap + heap_len - stack_size;
    unsigned char *sp = stack_base;

    /* Missing stack-limit check: pushes way beyond heap end. */
    for (unsigned long i = 0; i < heap_len; i++) {
        *sp++ = (unsigned char)(i & 0xff);
    }

    print_int(stack_base[0]);
}
EOF

############################################
# 122 – Heap-based Buffer Overflow
############################################
D122_DIR="${SRC_ROOT}/122_Heap_based_Buffer_Overflow"
mkdir -p "${D122_DIR}"
cat > "${D122_DIR}/122_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Simple bump "allocator" that ignores remaining heap and overruns it. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *heap_end = heap + heap_len;
    unsigned char *p = heap_end - 128;  /* pretend last allocation */

    unsigned long requested = 512;      /* too big for remaining heap */
    for (unsigned long i = 0; i < requested; i++) {
        p[i] = (unsigned char)(i & 0xff);  /* crosses linear memory end */
    }

    print_int(p[0]);
}
EOF

############################################
# 124 – Buffer Underwrite (Underflow)
############################################
D124_DIR="${SRC_ROOT}/124_Buffer_Underwrite"
mkdir -p "${D124_DIR}"
cat > "${D124_DIR}/124_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Writes before the beginning of the heap region. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    (void)wasm_pages(); /* unused in this pattern */

    unsigned char *buf = heap + 256;
    unsigned char *p = buf - 512;  /* definitely before heap base */

    for (int i = 0; i < 64; i++) {
        p[i] = (unsigned char)(i & 0xff);  /* underflow into unmapped region */
    }

    print_int(buf[0]);
}
EOF

############################################
# 125 – Out-of-bounds Read
############################################
D125_DIR="${SRC_ROOT}/125_Out_of_bounds_Read"
mkdir -p "${D125_DIR}"
cat > "${D125_DIR}/125_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Reads past the end of linear memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *p = heap + heap_len + 32;  /* beyond linear memory */
    int v = p[0];  /* out-of-bounds read */

    print_int(v);
}
EOF

############################################
# 126 – Buffer Over-read
############################################
D126_DIR="${SRC_ROOT}/126_Buffer_Over_read"
mkdir -p "${D126_DIR}"
cat > "${D126_DIR}/126_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* "Scan" past a logical buffer end into unmapped memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    unsigned long logical_len = 64;
    unsigned long scan_len = heap_len;  /* badly larger */

    int sum = 0;
    for (unsigned long i = 0; i < scan_len; i++) {
        sum += buf[i];  /* crosses end of linear memory */
    }

    print_int(sum);
}
EOF

############################################
# 127 – Buffer Under-read
############################################
D127_DIR="${SRC_ROOT}/127_Buffer_Under_read"
mkdir -p "${D127_DIR}"
cat > "${D127_DIR}/127_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Reads before the start of a heap buffer, below heap base. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    (void)wasm_pages();

    unsigned char *buf = heap + 256;
    int sum = 0;

    for (int i = -64; i < 64; i++) {
        unsigned char *p = buf + i;
        sum += *p;   /* for i<0, dereference < heap base */
    }

    print_int(sum);
}
EOF

############################################
# 129 – Improper Validation of Array Index
############################################
D129_DIR="${SRC_ROOT}/129_Improper_Validation_of_Array_Index"
mkdir -p "${D129_DIR}"
cat > "${D129_DIR}/129_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Uses an index derived from heap size without clamping to array size. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Place array near the very end of heap. */
    unsigned long arr_size = 64;
    unsigned char *arr = heap + heap_len - arr_size;

    unsigned long idx = heap_len / 2;  /* nonsensical, far too large */

    /* "Validate" index incorrectly: only checks idx >= 0. */
    if ((long)idx >= 0) {
        arr[idx] = 0x29;  /* out-of-bounds write beyond linear memory */
    }

    print_int(arr[0]);
}
EOF

############################################
# 131 – Incorrect Calculation of Buffer Size
############################################
D131_DIR="${SRC_ROOT}/131_Incorrect_Calculation_of_Buffer_Size"
mkdir -p "${D131_DIR}"
cat > "${D131_DIR}/131_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Treats heap as array of 8-byte items but uses char indexing. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long items = heap_len / 8;   /* "element count" */
    unsigned long bytes_to_write = items * 8 + 64;  /* off by +64 */

    for (unsigned long i = 0; i < bytes_to_write; i++) {
        heap[i] = (unsigned char)(i & 0xff);  /* overruns end of heap */
    }

    print_int(heap[0]);
}
EOF

############################################
# 786 – Access Before Start of Buffer
############################################
D786_DIR="${SRC_ROOT}/786_Access_of_Memory_Location_Before_Start_of_Buffer"
mkdir -p "${D786_DIR}"
cat > "${D786_DIR}/786_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Buffer logically starts in the middle of heap, but we step backwards
   beyond heap base. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    int sum = 0;

    for (int i = 0; i < 512; i++) {
        unsigned char *p = buf - (unsigned long)i;
        sum += *p;  /* for large i, p < heap base */
    }

    print_int(sum);
}
EOF

############################################
# 787 – Out-of-bounds Write
############################################
D787_DIR="${SRC_ROOT}/787_Out_of_bounds_Write"
mkdir -p "${D787_DIR}"
cat > "${D787_DIR}/787_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Single write exactly at heap_len and beyond. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    heap[heap_len] = 0x7;   /* write one byte beyond linear memory */

    print_int((int)heap[0]);
}
EOF

############################################
# 788 – Access After End of Buffer
############################################
D788_DIR="${SRC_ROOT}/788_Access_of_Memory_Location_After_End_of_Buffer"
mkdir -p "${D788_DIR}"
cat > "${D788_DIR}/788_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Reads just past the end of heap. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *past_end = heap + heap_len;
    int v = past_end[1];  /* one byte after heap end */

    print_int(v);
}
EOF

############################################
# 805 – Buffer Access with Incorrect Length
############################################
D805_DIR="${SRC_ROOT}/805_Buffer_Access_with_Incorrect_Length_Value"
mkdir -p "${D805_DIR}"
cat > "${D805_DIR}/805_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Passes total heap_len instead of logical length for a sub-buffer. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    unsigned long logical_len = 128;
    unsigned long length_param = heap_len;  /* wrong */

    for (unsigned long i = 0; i < length_param; i++) {
        buf[i] = (unsigned char)(i & 0xff);  /* out of heap range */
    }

    print_int(buf[0]);
}
EOF

############################################
# 806 – Buffer Access Using Size of Source Buffer
############################################
D806_DIR="${SRC_ROOT}/806_Buffer_Access_Using_Size_of_Source_Buffer"
mkdir -p "${D806_DIR}"
cat > "${D806_DIR}/806_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Uses size of a "source" range that crosses heap end to copy into a
   destination region, pushing writes out of linear memory. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap + heap_len - 64;
    unsigned char *dst = heap + heap_len / 2;

    unsigned long src_size = heap_len;  /* nonsensical */
    for (unsigned long i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* crosses end of heap */
    }

    print_int(dst[0]);
}
EOF

############################################
# 823 – Use of Out-of-range Pointer Offset
############################################
D823_DIR="${SRC_ROOT}/823_Use_of_Out_of_range_Pointer_Offset"
mkdir -p "${D823_DIR}"
cat > "${D823_DIR}/823_2.c" <<'EOF'
#include "wasm_layout.h"
extern void print_int(int);

/* Builds a pointer by adding a huge offset to a base inside heap. */
void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *base = heap + heap_len / 2;
    unsigned long offset = heap_len;  /* out-of-range offset */

    unsigned char *p = base + offset;  /* definitely beyond linear memory */
    int v = *p;  /* out-of-range pointer dereference */

    print_int(v);
}
EOF

echo "init_c.sh: generated v2 heap/linear-memory OOB CWEs under ${SRC_ROOT}"
