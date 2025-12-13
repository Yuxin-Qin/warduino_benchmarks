#!/usr/bin/env bash
set -euo pipefail

# Where this script lives, and project root (parent).
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
CROOT="${ROOT_DIR}/c"

echo "Project root: ${ROOT_DIR}"
echo "C root      : ${CROOT}"
echo

mkdir -p "${CROOT}"

###############################################################################
# 1) wasm_layout.h
###############################################################################
HDR="${CROOT}/wasm_layout.h"
echo "Writing ${HDR}"

cat > "${HDR}" <<'EOF'
/* wasm_layout.h
 *
 * Common helpers for page-aware WebAssembly benchmarks.
 * No standard library is used.
 */

#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

/* WebAssembly linear memory page size (fixed by spec). */
#define WASM_PAGE_SIZE 0x10000u

/* Provided by wasm-ld / runtime: start of the heap in linear memory. */
extern unsigned char __heap_base[];

/* Return number of pages in memory 0 at runtime. */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

/* Heap base pointer. */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

#endif /* WASM_LAYOUT_H */
EOF

###############################################################################
# Helper: write one C file
###############################################################################
write_cwe_file() {
    local rel_dir="$1"   # e.g., 119_Improper_Restriction_of_...
    local fname="$2"     # e.g., 119_1.c
    local body="$3"

    local dir="${CROOT}/${rel_dir}"
    local path="${dir}/${fname}"

    echo "Writing ${path}"
    mkdir -p "${dir}"
    cat > "${path}" <<'EOF'
EOF
    # Now append the body (passed as a here-doc string)
    printf '%s\n' "${body}" >> "${path}"
}

###############################################################################
# 2) CWE-119: Improper Restriction of Operations within the Bounds of a Buffer
###############################################################################
CWE119_DIR="119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer"
CWE119_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();

    /* Treat entire heap as one big buffer of length heap_len. */
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Incorrect sanitised length: off by +16 beyond heap. */
    unsigned long len = heap_len + 16;

    /* Copy loop uses len as if it were safe. */
    for (unsigned long i = 0; i < len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    sink = heap[0];
}
'
write_cwe_file "${CWE119_DIR}" "119_1.c" "${CWE119_SRC}"

###############################################################################
# 3) CWE-124: Buffer Underwrite (Buffer Underflow)
###############################################################################
CWE124_DIR="124_Buffer_Underwrite"
CWE124_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Define a sub-buffer inside the heap. */
    unsigned long offset = heap_len / 4;
    unsigned long size   = heap_len / 8;

    unsigned char *buf = heap + offset;      /* buffer [buf, buf+size) */

    /* Underwrite: step before buf by 32 bytes. */
    unsigned char *under = buf - 32;

    for (unsigned long i = 0; i < 64; i++) {
        under[i] = (unsigned char)i;         /* may fall below heap base */
    }

    sink = under[0];
}
'
write_cwe_file "${CWE124_DIR}" "124_1.c" "${CWE124_SRC}"

###############################################################################
# 4) CWE-125: Out-of-bounds Read
###############################################################################
CWE125_DIR="125_Out_of_bounds_Read"
CWE125_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Point near end of heap, then read beyond. */
    unsigned char *p = heap + heap_len - 8;

    /* Read 32 bytes, last 24 bytes are out-of-bounds. */
    unsigned char sum = 0;
    for (int i = 0; i < 32; i++) {
        sum ^= p[i];
    }

    sink = sum;
}
'
write_cwe_file "${CWE125_DIR}" "125_1.c" "${CWE125_SRC}"

###############################################################################
# 5) CWE-786: Access of Memory Location Before Start of Buffer
###############################################################################
CWE786_DIR="786_Access_of_Memory_Location_Before_Start_of_Buffer"
CWE786_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Define a buffer window that starts well inside heap. */
    unsigned long offset = heap_len / 2;
    unsigned long size   = 256;

    unsigned char *buf = heap + offset;

    /* Indexing uses signed index and allows negative values. */
    for (int i = -64; i < (int)size; i++) {
        unsigned char *p = buf + i;  /* for i < 0, p < buf */
        *p = (unsigned char)(i & 0xff);
    }

    sink = buf[0];
}
'
write_cwe_file "${CWE786_DIR}" "786_1.c" "${CWE786_SRC}"

###############################################################################
# 6) CWE-787: Out-of-bounds Write
###############################################################################
CWE787_DIR="787_Out_of_bounds_Write"
CWE787_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* buffer is last 1 KiB of heap */
    unsigned long buf_size = 1024;
    unsigned char *buf = heap + heap_len - buf_size;

    /* Incorrect loop bound: writes buf_size + 64 bytes. */
    for (unsigned long i = 0; i < buf_size + 64; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    sink = buf[0];
}
'
write_cwe_file "${CWE787_DIR}" "787_1.c" "${CWE787_SRC}"

###############################################################################
# 7) CWE-788: Access of Memory Location After End of Buffer
###############################################################################
CWE788_DIR="788_Access_of_Memory_Location_After_End_of_Buffer"
CWE788_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Sub-buffer in middle of heap. */
    unsigned long offset = heap_len / 3;
    unsigned long size   = 128;

    unsigned char *buf = heap + offset;

    /* Correctly write within the buffer. */
    for (unsigned long i = 0; i < size; i++) {
        buf[i] = (unsigned char)i;
    }

    /* Later buggy read: assumes size 256. */
    unsigned char sum = 0;
    for (unsigned long i = 0; i < 256; i++) {
        sum ^= buf[i];
    }

    sink = sum;
}
'
write_cwe_file "${CWE788_DIR}" "788_1.c" "${CWE788_SRC}"

###############################################################################
# 8) CWE-789: Memory Allocation with Excessive Size Value
###############################################################################
CWE789_DIR="789_Memory_Allocation_with_Excessive_Size_Value"
CWE789_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;
static unsigned long alloc_offset = 0;

static unsigned char *my_alloc(unsigned long n) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Bug: allow n that pushes us just past heap end. */
    unsigned long max_safe = heap_len - alloc_offset;
    unsigned long requested = n;

    if (requested > max_safe + 32) {
        /* still accept too-large request */
        requested = max_safe + 32;
    }

    unsigned char *p = heap + alloc_offset;
    alloc_offset += requested;
    return p;
}

void start(void) {
    /* Ask for a huge block near end of memory. */
    unsigned char *p = my_alloc(0x7fffffff);

    /* Write a small region, but p itself may already be out of bounds. */
    for (int i = 0; i < 64; i++) {
        p[i] = (unsigned char)i;
    }

    sink = p[0];
}
'
write_cwe_file "${CWE789_DIR}" "789_1.c" "${CWE789_SRC}"

###############################################################################
# 9) CWE-805: Buffer Access with Incorrect Length Value
###############################################################################
CWE805_DIR="805_Buffer_Access_with_Incorrect_Length_Value"
CWE805_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long buf_offset = heap_len / 4;
    unsigned long buf_size   = 512;
    unsigned char *buf = heap + buf_offset;

    /* Actual number of elements in buf is buf_size. */
    unsigned long actual_len = buf_size;

    /* Incorrect length used in loop (off by +256). */
    unsigned long used_len = actual_len + 256;

    for (unsigned long i = 0; i < used_len; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    sink = buf[0];
}
'
write_cwe_file "${CWE805_DIR}" "805_1.c" "${CWE805_SRC}"

###############################################################################
# 10) CWE-806: Buffer Access Using Size of Source Buffer
###############################################################################
CWE806_DIR="806_Buffer_Access_Using_Size_of_Source_Buffer"
CWE806_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Two buffers: small dst, large src. */
    unsigned long dst_size = 256;
    unsigned long src_size = 1024;

    unsigned long dst_off = heap_len / 8;
    unsigned long src_off = dst_off + dst_size;  /* src right after dst */

    unsigned char *dst = heap + dst_off;
    unsigned char *src = heap + src_off;

    /* Initialise src. */
    for (unsigned long i = 0; i < src_size; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    /* Bug: copy using src_size instead of dst_size. */
    for (unsigned long i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* writes past dst, potentially off end of heap */
    }

    sink = dst[0];
}
'
write_cwe_file "${CWE806_DIR}" "806_1.c" "${CWE806_SRC}"

###############################################################################
# 11) CWE-823: Use of Out-of-range Pointer Offset
###############################################################################
CWE823_DIR="823_Use_of_Out_of_range_Pointer_Offset"
CWE823_SRC='
#include "wasm_layout.h"

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = wasm_heap_base();
    int pages = wasm_pages();
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Base inside heap. */
    unsigned char *base = heap + heap_len / 2;

    /* Compute an out-of-range offset deliberately beyond heap end. */
    long offset = (long)heap_len;   /* moves pointer beyond heap upper bound */

    unsigned char *p = base + offset;  /* out-of-range pointer offset */

    /* Single dereference; on CHERI this should fault, on non-CHERI it is UB. */
    sink = *p;
}
'
write_cwe_file "${CWE823_DIR}" "823_1.c" "${CWE823_SRC}"

echo
echo "Done. Header and CWE C benchmarks generated under: ${CROOT}"
