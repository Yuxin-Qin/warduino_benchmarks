#!/usr/bin/env bash
set -euo pipefail

# Root of the repo (script is in ./script)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_ROOT="${ROOT_DIR}/c"

mkdir -p "${SRC_ROOT}"

# Helper: make dir and write a single C file via here-doc
write_c() {
  local dir="$1"
  local file="$2"
  local body="$3"

  mkdir -p "${SRC_ROOT}/${dir}"
  cat > "${SRC_ROOT}/${dir}/${file}" <<'EOF'
#define WASM_PAGE_SIZE 0x10000  /* 64 KiB */

extern unsigned char __heap_base[];
extern void print_int(int);

EOF
  # append the specific body
  printf "%s\n" "${body}" >> "${SRC_ROOT}/${dir}/${file}"
}

############################
# 119 – Improper Restriction
############################

write_c \
  "119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer" \
  "119_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    print_int(pages);
    print_int((int)heap);

    unsigned long len = heap_len + 16;
    for (unsigned long i = 0; i < len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }
}'

############################
# 120 – Classic buffer copy
############################

write_c \
  "120_Buffer_Copy_without_Checking_Size_of_Input" \
  "120_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *dst = heap + heap_len / 2;
    unsigned char *src = heap;

    unsigned long copy_len = heap_len;  /* overflows beyond end of heap */
    for (unsigned long i = 0; i < copy_len; i++) {
        dst[i] = src[i];
    }

    print_int(dst[0]);
}'

############################
# 121 – Stack-based overflow (simulated in linear memory)
############################

write_c \
  "121_Stack_based_Buffer_Overflow" \
  "121_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *stack_frame = heap + heap_len - 32;

    for (int i = 0; i < 64; i++) {
        stack_frame[i] = (unsigned char)i;  /* writes past end of linear memory */
    }

    print_int(stack_frame[0]);
}'

############################
# 122 – Heap-based overflow
############################

write_c \
  "122_Heap_based_Buffer_Overflow" \
  "122_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *heap_obj = heap + heap_len / 4;
    unsigned long obj_size = heap_len / 2;

    for (unsigned long i = 0; i < obj_size + 128; i++) {
        heap_obj[i] = (unsigned char)(i & 0xff);
    }

    print_int(heap_obj[0]);
}'

############################
# 124 – Buffer underwrite
############################

write_c \
  "124_Buffer_Underwrite" \
  "124_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;

    for (int i = -64; i < 64; i++) {
        unsigned char *p = buf + i;   /* for i<0, p < heap_base → underflow */
        *p = (unsigned char)(i & 0xff);
    }

    print_int(buf[0]);
}'

############################
# 125 – Out-of-bounds read
############################

write_c \
  "125_Out_of_bounds_Read" \
  "125_1.c" \
'volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *p = heap + heap_len + 32;  /* clearly past end of linear mem */
    sink = *p;
}'

############################
# 126 – Buffer over-read
############################

write_c \
  "126_Buffer_Over_read" \
  "126_1.c" \
'volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 2;
    unsigned long len = heap_len / 2 + 64;

    for (unsigned long i = 0; i < len; i++) {
        sink = buf[i];
    }
}'

############################
# 127 – Buffer under-read
############################

write_c \
  "127_Buffer_Under_read" \
  "127_1.c" \
'volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf = heap + 128;

    for (int i = -64; i < 0; i++) {
        unsigned char *p = buf + i;  /* reads before start of buffer, may cross below heap */
        sink = *p;
    }
}'

############################
# 129 – Improper index validation
############################

write_c \
  "129_Improper_Validation_of_Array_Index" \
  "129_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap;
    int idx = (int)heap_len;  /* use page-derived value as index */

    buf[idx] = 0x29;  /* well past linear memory */
    print_int(buf[0]);
}'

############################
# 131 – Incorrect buffer size calc
############################

write_c \
  "131_Incorrect_Calculation_of_Buffer_Size" \
  "131_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);

    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;
    unsigned long buf_len  = heap_len + 256;  /* miscomputed size */

    for (unsigned long i = 0; i < buf_len; i++) {
        heap[i] = (unsigned char)(i & 0xff);
    }

    print_int(heap[0]);
}'

############################
# 786 – Access before start
############################

write_c \
  "786_Access_of_Memory_Location_Before_Start_of_Buffer" \
  "786_1.c" \
'volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    unsigned char *buf = heap + 256;

    for (int i = -512; i < 0; i++) {
        unsigned char *p = buf + i;  /* can go below linear memory start */
        sink = *p;
    }
}'

############################
# 787 – Out-of-bounds write
############################

write_c \
  "787_Out_of_bounds_Write" \
  "787_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    for (unsigned long i = 0; i <= heap_len + 32; i++) {
        heap[i] = (unsigned char)(i & 0xff);  /* last writes beyond linear memory */
    }
}'

############################
# 788 – After end of buffer
############################

write_c \
  "788_Access_of_Memory_Location_After_End_of_Buffer" \
  "788_1.c" \
'volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned long buf_len = heap_len / 2;
    unsigned char *buf = heap;

    unsigned char *p = buf + buf_len + 128;  /* beyond allocated region and possibly linear mem */
    sink = *p;
}'

############################
# 805 – Incorrect length
############################

write_c \
  "805_Buffer_Access_with_Incorrect_Length_Value" \
  "805_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *buf = heap + heap_len / 3;
    unsigned long len = heap_len;  /* incorrect length relative to buf */

    for (unsigned long i = 0; i < len; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    print_int(buf[0]);
}'

############################
# 806 – Using size of source buffer
############################

write_c \
  "806_Buffer_Access_Using_Size_of_Source_Buffer" \
  "806_1.c" \
'void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *src  = heap;
    unsigned char *dest = heap + heap_len / 2;

    unsigned long src_len = heap_len;
    for (unsigned long i = 0; i < src_len; i++) {
        dest[i] = src[i];  /* dest runs off end of linear memory */
    }

    print_int(dest[0]);
}'

############################
# 823 – Out-of-range pointer offset
############################

write_c \
  "823_Use_of_Out_of_range_Pointer_Offset" \
  "823_1.c" \
'volatile unsigned char sink;

void start(void) {
    unsigned char *heap = __heap_base;
    int pages = __builtin_wasm_memory_size(0);
    unsigned long heap_len = (unsigned long)pages * WASM_PAGE_SIZE;

    unsigned char *base = heap + heap_len / 2;
    long offset = (long)heap_len;  /* push pointer beyond heap upper bound */

    unsigned char *p = base + offset;
    sink = *p;
}'

echo "init_c.sh: C benchmarks written under ${SRC_ROOT}"
