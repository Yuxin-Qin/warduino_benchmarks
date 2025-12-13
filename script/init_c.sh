#!/usr/bin/env bash
set -euo pipefail

# Directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Project root = parent of script dir
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"

OUT_DIR="${ROOT_DIR}/c/823_Use_of_Out_of_range_Pointer_Offset"

mkdir -p "${OUT_DIR}"

echo "Writing CWE-823 benchmarks to: ${OUT_DIR}"

########################################
# 823_1.c
########################################
cat > "${OUT_DIR}/823_1.c" <<'EOF'
/* 823_1.c – CWE-823: simple huge forward offset from heap_base */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    /* Diagnostics: how big is memory, where is heap base. */
    print_int(num_pages);
    print_int((int)heap_base);

    /* Base pointer into heap, then a huge forward offset in bytes. */
    volatile int *p = (int *)heap_base;

    /* Out-of-range pointer offset: stride crosses all pages and more. */
    int byte_offset = num_pages * WASM_PAGE_SIZE + 128;
    volatile int *bad = (int *)((unsigned char *)p + byte_offset);

    /* Misuse: write through an out-of-range pointer offset. */
    *bad = 0x82300001;

    /* Keep p “live” so compiler does not optimise it away. */
    *p = 1;
    print_int(*p);
}
EOF

########################################
# 823_2.c
########################################
cat > "${OUT_DIR}/823_2.c" <<'EOF'
/* 823_2.c – CWE-823: off-by-one page offset beyond heap region */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Compute pointer just past the end of linear memory. */
    unsigned char *end_of_memory =
        heap_base + num_pages * WASM_PAGE_SIZE;

    /* Off-by-one page: one extra page beyond legal region. */
    unsigned char *bad_byte_ptr = end_of_memory + WASM_PAGE_SIZE;
    volatile int *bad = (int *)bad_byte_ptr;

    *bad = 0x82300002;  /* Out-of-range write */

    /* Touch heap_base to keep it live. */
    heap_base[0] = 2;
    print_int((int)heap_base[0]);
}
EOF

########################################
# 823_3.c
########################################
cat > "${OUT_DIR}/823_3.c" <<'EOF'
/* 823_3.c – CWE-823: wrong struct field chosen as base + huge offset */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

struct Region {
    int header;
    int payload[8];
};

static struct Region region = { 0, { 0 } };

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Correct payload base would be &region.payload[0]. */
    volatile int *base = &region.header;  /* Wrong base (header) */

    /* Out-of-range offset scaled by pages. */
    int offset_words = (num_pages * (WASM_PAGE_SIZE / (int)sizeof(int))) + 32;
    volatile int *bad = base + offset_words;

    *bad = 0x82300003;       /* Out-of-range pointer offset use */

    region.payload[0] = 3;   /* Keep struct alive */
    print_int(region.payload[0]);
}
EOF

########################################
# 823_4.c
########################################
cat > "${OUT_DIR}/823_4.c" <<'EOF'
/* 823_4.c – CWE-823: negative offset stepping *before* heap_base */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Start from heap_base + some small positive offset. */
    unsigned char *p = heap_base + 64;

    /* Large negative offset: walk backwards across multiple pages. */
    int negative_bytes = num_pages * WASM_PAGE_SIZE + 256;
    unsigned char *bad_byte_ptr = p - negative_bytes;
    volatile int *bad = (int *)bad_byte_ptr;

    *bad = 0x82300004;  /* Out-of-range write before region */

    heap_base[1] = 4;
    print_int((int)heap_base[1]);
}
EOF

########################################
# 823_5.c
########################################
cat > "${OUT_DIR}/823_5.c" <<'EOF'
/* 823_5.c – CWE-823: using page count as an array index directly */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static int table[32];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Fill table with something deterministic. */
    for (int i = 0; i < 32; i++) {
        table[i] = i;
    }

    /* Misuse: treat num_pages as a valid table index. */
    int idx = num_pages;  /* Typically far beyond [0..31] */
    volatile int *bad = &table[idx];

    *bad = 0x82300005;  /* Out-of-range pointer offset use */

    /* Keep a valid entry alive. */
    print_int(table[0]);
}
EOF

########################################
# 823_6.c
########################################
cat > "${OUT_DIR}/823_6.c" <<'EOF'
/* 823_6.c – CWE-823: mixing byte and word counts in offset computation */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static int g_words[64];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    volatile int *base = g_words;

    /* Compute stride as if WASM_PAGE_SIZE already counted words. */
    int stride_words = num_pages * WASM_PAGE_SIZE;  /* actually bytes */

    /* base + stride_words now leaps far beyond g_words. */
    volatile int *bad = base + stride_words;

    *bad = 0x82300006;  /* Out-of-range write */

    g_words[1] = 6;
    print_int(g_words[1]);
}
EOF

########################################
# 823_7.c
########################################
cat > "${OUT_DIR}/823_7.c" <<'EOF'
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
EOF

########################################
# 823_8.c
########################################
cat > "${OUT_DIR}/823_8.c" <<'EOF'
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
EOF

########################################
# 823_9.c
########################################
cat > "${OUT_DIR}/823_9.c" <<'EOF'
/* 823_9.c – CWE-823: header/payload layout miscomputed using pages */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

struct Block {
    int header[4];
    int payload[4];
};

static struct Block block = { {0}, {0} };

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    /* Correct payload base would be &block.payload[0]. */
    volatile int *base = &block.payload[0];

    /* Miscompute out-of-range offset using page size as element count. */
    int wrong_offset = WASM_PAGE_SIZE * num_pages + 4;
    volatile int *bad = base + wrong_offset;

    *bad = 0x82300009;

    block.header[0] = 9;
    print_int(block.header[0]);
}
EOF

########################################
# 823_10.c
########################################
cat > "${OUT_DIR}/823_10.c" <<'EOF'
/* 823_10.c – CWE-823: pointer walks from heap into unrelated globals */

#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);

static int globals_a[4];
static int globals_b[4];

void start(void) {
    int num_pages = __builtin_wasm_memory_size(0);
    unsigned char *heap_base = __heap_base;

    print_int(num_pages);
    print_int((int)heap_base);

    for (int i = 0; i < 4; i++) {
        globals_a[i] = i;
        globals_b[i] = 100 + i;
    }

    /* Start in the heap region. */
    volatile int *base = (int *)heap_base;

    /*
     * Misuse: assume globals are “one page after” heap_base.
     * This fabricates a cross-region pointer that is out-of-range
     * relative to the capability region.
     */
    volatile int *bad = (int *)(heap_base + WASM_PAGE_SIZE * num_pages);
    bad += 8;  /* additional word offset */

    *bad = 0x82300010;

    print_int(globals_a[0]);
    print_int(globals_b[0]);
}
EOF

echo "Done."
