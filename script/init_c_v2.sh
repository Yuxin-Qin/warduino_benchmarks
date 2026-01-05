#!/usr/bin/env bash
set -euo pipefail

# Root paths (repo root = parent of this script)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_ROOT="${ROOT_DIR}/c"

mkdir -p "${SRC_ROOT}"

########################################
# Common header: wasm_layout.h
########################################
cat > "${SRC_ROOT}/wasm_layout.h" <<'EOF'
#ifndef WASM_LAYOUT_H
#define WASM_LAYOUT_H

#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

/* Start of linear heap as seen by C code compiled to Wasm. */
static inline unsigned char *wasm_heap_base(void) {
    return __heap_base;
}

/* Number of 64 KiB pages in the default Wasm memory. */
static inline int wasm_pages(void) {
    return __builtin_wasm_memory_size(0);
}

#endif /* WASM_LAYOUT_H */
EOF

########################################
# 119 – Improper Restriction of Operations within the Bounds of a Memory Buffer
########################################
DIR="${SRC_ROOT}/119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer"
mkdir -p "${DIR}"
cat > "${DIR}/119_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-119:
 * Treat the entire linear memory as one big "log buffer" and
 * perform a bulk copy of all bytes into the second half,
 * without restricting the operation to the remaining buffer size.
 */
void start(void) {
    unsigned char *heap     = wasm_heap_base();
    int            pages    = wasm_pages();
    unsigned long  heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + heap_len / 2;  /* only half of heap available */

    /* Caller requests a copy of the entire heap into the second half. */
    unsigned long requested = heap_len;

    /* Improper restriction: we never clamp requested to the dst capacity. */
    unsigned long copy_len = requested;

    for (unsigned long i = 0; i < copy_len; i++) {
        dst[i] = src[i];  /* out-of-bounds write once i >= heap_len/2 */
    }

    print_int(dst[0]);
}
EOF

########################################
# 120 – Buffer Copy without Checking Size of Input
########################################
DIR="${SRC_ROOT}/120_Buffer_Copy_without_Checking_Size_of_Input"
mkdir -p "${DIR}"
cat > "${DIR}/120_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-120:
 * Copy a "received packet" into a small fixed-size stack buffer,
 * using the packet length directly without checking against dest size.
 */
void start(void) {
    unsigned char packet[64];
    unsigned char dest[16];
    int           input_len = 64;  /* "received" length */
    int           checksum  = 0;

    for (int i = 0; i < 64; i++) {
        packet[i] = (unsigned char)(i & 0xff);
    }

    /* Vulnerable: blindly uses input_len. */
    for (int i = 0; i < input_len; i++) {
        dest[i] = packet[i];  /* overflow dest */
    }

    for (int i = 0; i < 16; i++) {
        checksum += dest[i];
    }

    print_int(checksum);
}
EOF

########################################
# 121 – Stack-based Buffer Overflow
########################################
DIR="${SRC_ROOT}/121_Stack_based_Buffer_Overflow"
mkdir -p "${DIR}"
cat > "${DIR}/121_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-121:
 * Build a stack log message and append a tag,
 * but never check the destination size.
 */
void start(void) {
    char log_buf[32];
    char tag[48];
    int  acc = 0;

    for (int i = 0; i < 32; i++) {
        log_buf[i] = 'A';
    }
    for (int i = 0; i < 48; i++) {
        tag[i] = (char)('a' + (i % 26));
    }

    /* Overflow log_buf. */
    for (int i = 0; i < 48; i++) {
        log_buf[i] = tag[i];
    }

    for (int i = 0; i < 32; i++) {
        acc += (int)log_buf[i];
    }

    print_int(acc);
}
EOF

########################################
# 122 – Heap-based Buffer Overflow
########################################
DIR="${SRC_ROOT}/122_Heap_based_Buffer_Overflow"
mkdir -p "${DIR}"
cat > "${DIR}/122_2.c" <<'EOF'
#include "wasm_layout.h"

static unsigned char *heap_ptr = 0;

static void init_heap(void) {
    if (!heap_ptr) {
        heap_ptr = wasm_heap_base();
    }
}

static void *my_alloc(unsigned long bytes) {
    init_heap();
    unsigned char *p = heap_ptr;
    heap_ptr += bytes;  /* no heap bound check */
    return (void *)p;
}

/*
 * CWE-122:
 * Allocate a message buffer but copy more bytes than allocated.
 */
void start(void) {
    unsigned char *src;
    unsigned char *dst;
    unsigned long  alloc_size = 32;
    unsigned long  to_copy    = 64;
    int            sum        = 0;

    src = (unsigned char *)my_alloc(64);
    dst = (unsigned char *)my_alloc(alloc_size);

    for (unsigned long i = 0; i < 64; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    /* Overflow dst. */
    for (unsigned long i = 0; i < to_copy; i++) {
        dst[i] = src[i];
    }

    for (unsigned long i = 0; i < alloc_size; i++) {
        sum += dst[i];
    }

    print_int(sum);
}
EOF

########################################
# 124 – Buffer Underwrite
########################################
DIR="${SRC_ROOT}/124_Buffer_Underwrite"
mkdir -p "${DIR}"
cat > "${DIR}/124_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-124:
 * Sliding window cursor moved too far backward, then written.
 */
void start(void) {
    unsigned char window[32];
    unsigned char *cursor = &window[8];
    int sum = 0;

    for (int i = 0; i < 32; i++) {
        window[i] = (unsigned char)i;
    }

    cursor -= 16;  /* before window[0] */
    for (int i = 0; i < 8; i++) {
        cursor[i] = (unsigned char)(i + 100);
    }

    for (int i = 0; i < 32; i++) {
        sum += window[i];
    }

    print_int(sum);
}
EOF

########################################
# 125 – Out-of-bounds Read
########################################
DIR="${SRC_ROOT}/125_Out_of_bounds_Read"
mkdir -p "${DIR}"
cat > "${DIR}/125_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-125:
 * Scan payload for sentinel but ignore declared length.
 */
void start(void) {
    unsigned char payload[32];
    int           len   = 16;
    int           found = -1;

    for (int i = 0; i < 32; i++) {
        payload[i] = (unsigned char)(i & 0xff);
    }
    payload[10] = 0xAA;

    for (int i = 0; i < 32; i++) {  /* ignores len */
        if (payload[i] == 0xAA) {
            found = i;
            break;
        }
    }

    print_int(found);
}
EOF

########################################
# 126 – Buffer Over-read
########################################
DIR="${SRC_ROOT}/126_Buffer_Over_read"
mkdir -p "${DIR}"
cat > "${DIR}/126_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-126:
 * Copy full capacity even though only 'received' bytes are valid.
 */
void start(void) {
    unsigned char src[64];
    unsigned char dst[64];
    int           received = 20;
    int           sum      = 0;

    for (int i = 0; i < 64; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    for (int i = 0; i < 64; i++) {  /* should use received */
        dst[i] = src[i];
    }

    for (int i = 0; i < 64; i++) {
        sum += dst[i];
    }

    print_int(sum);
}
EOF

########################################
# 127 – Buffer Under-read
########################################
DIR="${SRC_ROOT}/127_Buffer_Under_read"
mkdir -p "${DIR}"
cat > "${DIR}/127_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-127:
 * Sliding read window index goes negative conceptually.
 */
void start(void) {
    unsigned char buf[32];
    int           start_index = 8;
    int           sum         = 0;

    for (int i = 0; i < 32; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    start_index -= 16;  /* becomes negative logically */

    for (int i = start_index; i < start_index + 10; i++) {
        sum += buf[i];  /* under-read for i < 0 in C semantics */
    }

    print_int(sum);
}
EOF

########################################
# 129 – Improper Validation of Array Index
########################################
DIR="${SRC_ROOT}/129_Improper_Validation_of_Array_Index"
mkdir -p "${DIR}"
cat > "${DIR}/129_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-129:
 * Only checks index >= 0, not < size.
 */
void start(void) {
    int sensor_values[8];
    int sensor_id = 15;
    int reading   = -1;

    for (int i = 0; i < 8; i++) {
        sensor_values[i] = i * 10;
    }

    if (sensor_id >= 0) {
        reading = sensor_values[sensor_id];  /* out-of-bounds */
    }

    print_int(reading);
}
EOF

########################################
# 131 – Incorrect Calculation of Buffer Size
########################################
DIR="${SRC_ROOT}/131_Incorrect_Calculation_of_Buffer_Size"
mkdir -p "${DIR}"
cat > "${DIR}/131_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-131:
 * Mis-sized path buffer; no room for full string.
 */
void start(void) {
    char base[8]   = {'/','a','p','p','/',0,0,0};
    char suffix[8] = {'l','o','g','.','t','x','t',0};
    char path[12];
    int  i, idx = 0;
    int  acc = 0;

    for (i = 0; i < 8 && base[i] != 0; i++) {
        path[idx++] = base[i];
    }
    for (i = 0; i < 8 && suffix[i] != 0; i++) {
        path[idx++] = suffix[i];  /* may overflow path */
    }

    for (i = 0; i < 12; i++) {
        acc += (int)path[i];
    }

    print_int(acc);
}
EOF

########################################
# 786 – Access of Memory Location Before Start of Buffer
########################################
DIR="${SRC_ROOT}/786_Access_of_Memory_Location_Before_Start_of_Buffer"
mkdir -p "${DIR}"
cat > "${DIR}/786_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-786:
 * Window in middle of heap; loop uses negative index.
 */
void start(void) {
    unsigned char *heap     = wasm_heap_base();
    int            pages    = wasm_pages();
    unsigned long  heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long  offset = heap_len / 2;
    unsigned char *window = heap + offset;
    int            sum    = 0;

    for (int i = 0; i < 256; i++) {
        window[i] = (unsigned char)(i & 0xff);
    }

    for (int i = -32; i < 64; i++) {
        unsigned char *p = window + i;  /* may underflow window */
        sum += *p;
    }

    print_int(sum);
}
EOF

########################################
# 787 – Out-of-bounds Write
########################################
DIR="${SRC_ROOT}/787_Out_of_bounds_Write"
mkdir -p "${DIR}"
cat > "${DIR}/787_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-787:
 * Off-by-one log index check.
 */
void start(void) {
    int  log_buf[16];
    int  count = 16;
    int  acc   = 0;

    for (int i = 0; i < 16; i++) {
        log_buf[i] = i;
    }

    if (count <= 16) {
        log_buf[count] = 999;  /* log_buf[16] is out-of-bounds */
        count++;
    }

    for (int i = 0; i < 16; i++) {
        acc += log_buf[i];
    }

    print_int(acc);
}
EOF

########################################
# 788 – Access of Memory Location After End of Buffer
########################################
DIR="${SRC_ROOT}/788_Access_of_Memory_Location_After_End_of_Buffer"
mkdir -p "${DIR}"
cat > "${DIR}/788_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-788:
 * Read terminator one past end of buffer.
 */
void start(void) {
    unsigned char buf[16];
    int           acc = 0;

    for (int i = 0; i < 16; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    unsigned char terminator = buf[16];  /* out-of-bounds read */
    acc += (int)terminator;

    for (int i = 0; i < 16; i++) {
        acc += buf[i];
    }

    print_int(acc);
}
EOF

########################################
# 805 – Buffer Access with Incorrect Length Value
########################################
DIR="${SRC_ROOT}/805_Buffer_Access_with_Incorrect_Length_Value"
mkdir -p "${DIR}"
cat > "${DIR}/805_2.c" <<'EOF'
#include "wasm_layout.h"

static int process_payload(unsigned char *buf, unsigned long len) {
    int sum = 0;
    for (unsigned long i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}

/*
 * CWE-805:
 * Caller passes length too large; callee trusts it.
 */
void start(void) {
    unsigned char buf[32];
    unsigned long real_len  = 16;
    unsigned long wrong_len = 64;
    (void)real_len;

    for (int i = 0; i < 32; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    int result = process_payload(buf, wrong_len);
    print_int(result);
}
EOF

########################################
# 806 – Buffer Access Using Size of Source Buffer
########################################
DIR="${SRC_ROOT}/806_Buffer_Access_Using_Size_of_Source_Buffer"
mkdir -p "${DIR}"
cat > "${DIR}/806_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-806:
 * Uses size of src when copying into smaller dst.
 */
void start(void) {
    unsigned char src[64];
    unsigned char dst[16];
    int           sum = 0;

    for (int i = 0; i < 64; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    for (int i = 0; i < 64; i++) {  /* should cap at 16 */
        dst[i] = src[i];
    }

    for (int i = 0; i < 16; i++) {
        sum += dst[i];
    }

    print_int(sum);
}
EOF

########################################
# 823 – Use of Out-of-range Pointer Offset
########################################
DIR="${SRC_ROOT}/823_Use_of_Out_of_range_Pointer_Offset"
mkdir -p "${DIR}"
cat > "${DIR}/823_2.c" <<'EOF'
#include "wasm_layout.h"

/*
 * CWE-823:
 * Compute pointer beyond entire heap and dereference once.
 */
static volatile unsigned char sink;

void start(void) {
    unsigned char *heap     = wasm_heap_base();
    int            pages    = wasm_pages();
    unsigned long  heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *base = heap + heap_len / 2;
    long           offset = (long)heap_len;

    unsigned char *p = base + offset;  /* out-of-range offset */

    sink = *p;
    print_int(pages);
}
EOF

echo "init_c_v2.sh: generated wasm_layout.h and *_2.c benchmarks under c/"
