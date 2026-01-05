#!/usr/bin/env bash
set -euo pipefail

# Root paths (repo root = parent of this script)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_ROOT="$ROOT_DIR/c"

mkdir -p "$SRC_ROOT"

mk() {
  local dir="$1"
  local file="$2"
  mkdir -p "$SRC_ROOT/$dir"
  cat > "$SRC_ROOT/$dir/$file.c" <<'EOF'
EOF
}

########################
# 119 – Improper Restriction of Operations within the Bounds of a Memory Buffer
########################
mkdir -p "$SRC_ROOT/119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer"
cat > "$SRC_ROOT/119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer/119_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Incorrect bound: allow writes slightly beyond linear memory. */
    unsigned long len = heap_len + 16;

    for (unsigned long i = 0; i < len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    print_string("119_1 done\n", 11);
}
EOF

########################
# 120 – Buffer Copy without Checking Size of Input
########################
mkdir -p "$SRC_ROOT/120_Buffer_Copy_without_Checking_Size_of_Input"
cat > "$SRC_ROOT/120_Buffer_Copy_without_Checking_Size_of_Input/120_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *dst = heap + (heap_len / 2);
    unsigned char *src = heap;

    /* Copy beyond end of linear memory. */
    unsigned long len = (heap_len / 2) + 32;

    for (unsigned long i = 0; i < len; i++) {
        dst[i] = src[i];
    }

    print_string("120_1 done\n", 11);
}
EOF

########################
# 121 – Stack-based Buffer Overflow
# (simulated via linear memory region treated like a stack)
########################
mkdir -p "$SRC_ROOT/121_Stack_based_Buffer_Overflow"
cat > "$SRC_ROOT/121_Stack_based_Buffer_Overflow/121_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Treat top quarter of heap as "stack frame". */
    unsigned char *frame = heap + (heap_len * 3) / 4;
    unsigned long frame_size = heap_len / 8;

    /* Overflow frame upward past end of memory. */
    for (unsigned long i = 0; i < frame_size + 64; i++) {
        frame[i] = (unsigned char)(i & 0xff);
    }

    print_string("121_1 done\n", 11);
}
EOF

########################
# 122 – Heap-based Buffer Overflow
########################
mkdir -p "$SRC_ROOT/122_Heap_based_Buffer_Overflow"
cat > "$SRC_ROOT/122_Heap_based_Buffer_Overflow/122_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

static unsigned char *fake_heap = 0;
static unsigned long fake_size  = 0;

static unsigned char *my_malloc(unsigned long n) {
    if (!fake_heap) {
        fake_heap = __heap_base;
        int pages = __builtin_wasm_memory_size(0);
        fake_size = (unsigned long)pages * WASM_PAGE_SIZE;
    }
    return fake_heap; /* one big region */
}

void start(void) {
    unsigned char *buf = my_malloc(64);
    unsigned long n = fake_size + 32; /* overflow beyond allocated region */

    for (unsigned long i = 0; i < n; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    print_string("122_1 done\n", 11);
}
EOF

########################
# 124 – Buffer Underwrite (Buffer Underflow)
########################
mkdir -p "$SRC_ROOT/124_Buffer_Underwrite"
cat > "$SRC_ROOT/124_Buffer_Underwrite/124_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf  = heap + 128;

    /* Write before start of buffer; if heap == linear start this may
       also cross below linear memory on small configs. */
    for (int i = -256; i < 0; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    print_string("124_1 done\n", 11);
}
EOF

########################
# 125 – Out-of-bounds Read
########################
mkdir -p "$SRC_ROOT/125_Out_of_bounds_Read"
cat > "$SRC_ROOT/125_Out_of_bounds_Read/125_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    for (unsigned long i = 0; i < heap_len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    /* Read past end of linear memory. */
    unsigned char v = heap[heap_len + 8];
    print_int((int)v);

    print_string("125_1 done\n", 11);
}
EOF

########################
# 126 – Buffer Over-read
########################
mkdir -p "$SRC_ROOT/126_Buffer_Over_read"
cat > "$SRC_ROOT/126_Buffer_Over_read/126_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + (heap_len / 4);
    unsigned long len  = (heap_len / 4) + 32;

    for (unsigned long i = 0; i < len; i++) {
        unsigned char v = buf[i];
        print_int((int)v);
    }

    print_string("126_1 done\n", 11);
}
EOF

########################
# 127 – Buffer Under-read
########################
mkdir -p "$SRC_ROOT/127_Buffer_Under_read"
cat > "$SRC_ROOT/127_Buffer_Under_read/127_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf  = heap + 64;

    for (int i = -64; i < 64; i++) {
        unsigned char *p = buf + i;
        unsigned char v  = *p;
        print_int((int)v);
    }

    print_string("127_1 done\n", 11);
}
EOF

########################
# 129 – Improper Validation of Array Index
########################
mkdir -p "$SRC_ROOT/129_Improper_Validation_of_Array_Index"
cat > "$SRC_ROOT/129_Improper_Validation_of_Array_Index/129_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

static int table[32];

void start(void) {
    int pages = __builtin_wasm_memory_size(0);
    for (int i = 0; i < 32; i++) {
        table[i] = i;
    }

    /* Use pages directly as index, far beyond [0..31]. */
    table[pages] = 0x12900001;

    print_string("129_1 done\n", 11);
}
EOF

########################
# 131 – Incorrect Calculation of Buffer Size
########################
mkdir -p "$SRC_ROOT/131_Incorrect_Calculation_of_Buffer_Size"
cat > "$SRC_ROOT/131_Incorrect_Calculation_of_Buffer_Size/131_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);

    /* Wrong size: assume only 1 page regardless of actual. */
    unsigned long wrong_len = WASM_PAGE_SIZE;

    /* But still loop up to pages*page_size, possibly past real memory. */
    unsigned long real_len = (unsigned long)pages * WASM_PAGE_SIZE;

    for (unsigned long i = 0; i < real_len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    (void)wrong_len;

    print_string("131_1 done\n", 11);
}
EOF

########################
# 786 – Access of Memory Location Before Start of Buffer
########################
mkdir -p "$SRC_ROOT/786_Access_of_Memory_Location_Before_Start_of_Buffer"
cat > "$SRC_ROOT/786_Access_of_Memory_Location_Before_Start_of_Buffer/786_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned long offset = 256;

    unsigned char *buf = heap + offset;

    for (int i = -512; i < 0; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    print_string("786_1 done\n", 11);
}
EOF

########################
# 787 – Out-of-bounds Write
########################
mkdir -p "$SRC_ROOT/787_Out_of_bounds_Write"
cat > "$SRC_ROOT/787_Out_of_bounds_Write/787_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    for (unsigned long i = 0; i <= heap_len + 32; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    print_string("787_1 done\n", 11);
}
EOF

########################
# 788 – Access of Memory Location After End of Buffer
########################
mkdir -p "$SRC_ROOT/788_Access_of_Memory_Location_After_End_of_Buffer"
cat > "$SRC_ROOT/788_Access_of_Memory_Location_After_End_of_Buffer/788_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_int(int);
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + (heap_len / 2);
    unsigned long size  = 256;

    for (unsigned long i = 0; i < size; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    unsigned char v = buf[size + (heap_len / 2)]; /* well past end of heap */
    print_int((int)v);

    print_string("788_1 done\n", 11);
}
EOF

########################
# 805 – Buffer Access with Incorrect Length Value
########################
mkdir -p "$SRC_ROOT/805_Buffer_Access_with_Incorrect_Length_Value"
cat > "$SRC_ROOT/805_Buffer_Access_with_Incorrect_Length_Value/805_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + 64;
    unsigned long real_len = 128;
    unsigned long wrong_len = heap_len;  /* incorrect length */

    for (unsigned long i = 0; i < wrong_len; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    (void)real_len;

    print_string("805_1 done\n", 11);
}
EOF

########################
# 806 – Buffer Access Using Size of Source Buffer
########################
mkdir -p "$SRC_ROOT/806_Buffer_Access_Using_Size_of_Source_Buffer"
cat > "$SRC_ROOT/806_Buffer_Access_Using_Size_of_Source_Buffer/806_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src = heap;
    unsigned char *dst = heap + (heap_len / 2);

    unsigned long src_size = heap_len;       /* full linear memory */
    unsigned long dst_size = heap_len / 4;   /* smaller region */

    for (unsigned long i = 0; i < src_size; i++) {
        dst[i] = src[i];  /* uses src size, overruns dst */
    }

    (void)dst_size;

    print_string("806_1 done\n", 11);
}
EOF

########################
# 823 – Use of Out-of-range Pointer Offset
########################
mkdir -p "$SRC_ROOT/823_Use_of_Out_of_range_Pointer_Offset"
cat > "$SRC_ROOT/823_Use_of_Out_of_range_Pointer_Offset/823_1.c" <<'EOF'
#define WASM_PAGE_SIZE 0x10000

extern unsigned char __heap_base[];
extern void print_string(const char *s, int len);

volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    /* Base inside heap. */
    unsigned char *base = heap + heap_len / 2;

    /* Offset that walks beyond linear memory. */
    long offset = (long)heap_len;

    unsigned char *p = base + offset;  /* out-of-range pointer offset */

    sink = *p;

    print_string("823_1 done\n", 11);
}
EOF

echo "init_c.sh: generated all CWE benchmarks."
