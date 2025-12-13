#!/usr/bin/env bash
set -euo pipefail

# Root of the benchmark project (this script is assumed in ./script)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
SRC_ROOT="${ROOT_DIR}/c"

mkdir -p "${SRC_ROOT}"

# Helper to create dir and C file
write_c() {
  local dir="$1"
  local file="$2"
  local body="$3"
  mkdir -p "${dir}"
  cat > "${dir}/${file}" <<EOF
${body}
EOF
}

###############################################################################
# 119 – Improper Restriction of Operations within the Bounds of a Memory Buffer
###############################################################################
DIR_119="${SRC_ROOT}/119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer"
write_c "${DIR_119}" "119.c" "/* CWE-119 */
volatile int sink;
void start(void) {
    char buf[16];
    int len = 24;  /* overly permissive length */
    for (int i = 0; i < len; i++) {
        buf[i] = (char)i;  /* may exceed buf[15] */
    }
    sink = buf[0];
}
"

###############################################################################
# 120 – Buffer Copy without Checking Size of Input
###############################################################################
DIR_120="${SRC_ROOT}/120_Buffer_Copy_without_Checking_Size_of_Input"
write_c "${DIR_120}" "120.c" "/* CWE-120 */
volatile int sink;
void start(void) {
    char src[32];
    char dst[8];
    int len = 20;  /* attacker controlled */
    for (int i = 0; i < len; i++) {
        src[i] = (char)i;
    }
    /* No check that len <= sizeof(dst) */
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];  /* overflow of dst */
    }
    sink = dst[0];
}
"

###############################################################################
# 121 – Stack-based Buffer Overflow
###############################################################################
DIR_121="${SRC_ROOT}/121_Stack_based_Buffer_Overflow"
write_c "${DIR_121}" "121.c" "/* CWE-121 */
volatile int guard = 12345;
void start(void) {
    int buf[8];
    for (int i = 0; i < 16; i++) {
        buf[i] = 0x1000 + i;  /* writes beyond buf[7] */
    }
    (void)guard;
}
"

###############################################################################
# 122 – Heap-based Buffer Overflow (tiny custom heap)
###############################################################################
DIR_122="${SRC_ROOT}/122_Heap_based_Buffer_Overflow"
write_c "${DIR_122}" "122.c" "/* CWE-122 */
volatile int sink;
static char heap_area[32];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}

void start(void) {
    char *p = (char *)my_alloc(8);
    if (!p) return;
    for (int i = 0; i < 16; i++) {
        p[i] = (char)i;   /* overflow past allocated 8 bytes */
    }
    sink = p[0];
}
"

###############################################################################
# 123 – Write-what-where Condition
###############################################################################
DIR_123="${SRC_ROOT}/123_Write_what_where_Condition"
write_c "${DIR_123}" "123.c" "/* CWE-123 */
volatile int victim = 0;
volatile int *target_ptr = &victim;
volatile int attacker_value = 0x5555;

void start(void) {
    volatile int **pp = (volatile int **)&target_ptr;
    /* attacker corrupts pointer */
    *pp = (volatile int *)((unsigned long)pp + 16);
    **pp = attacker_value;   /* uncontrolled write-what-where */
}
"

###############################################################################
# 124 – Buffer Underwrite (Buffer Underflow)
###############################################################################
DIR_124="${SRC_ROOT}/124_Buffer_Underwrite"
write_c "${DIR_124}" "124.c" "/* CWE-124 */
volatile int sink;
void start(void) {
    char buf[16];
    char *p = &buf[4];
    for (int i = 0; i < 8; i++) {
        p[-(i+1)] = (char)i;  /* write before buf[0] */
    }
    sink = buf[0];
}
"

###############################################################################
# 125 – Out-of-bounds Read
###############################################################################
DIR_125="${SRC_ROOT}/125_Out_of_bounds_Read"
write_c "${DIR_125}" "125.c" "/* CWE-125 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (char)i;
    char v = buf[10];   /* read beyond end */
    sink = (int)v;
}
"

###############################################################################
# 126 – Buffer Over-read
###############################################################################
DIR_126="${SRC_ROOT}/126_Buffer_Over_read"
write_c "${DIR_126}" "126.c" "/* CWE-126 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (char)i;
    char last = 0;
    for (int i = 0; i < 16; i++) {
        last = buf[i];  /* reads past end */
    }
    sink = (int)last;
}
"

###############################################################################
# 127 – Buffer Under-read
###############################################################################
DIR_127="${SRC_ROOT}/127_Buffer_Under_read"
write_c "${DIR_127}" "127.c" "/* CWE-127 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (char)(i + 1);
    char *p = &buf[2];
    char v = p[-4];  /* read before buf[0] */
    sink = (int)v;
}
"

###############################################################################
# 129 – Improper Validation of Array Index
###############################################################################
DIR_129="${SRC_ROOT}/129_Improper_Validation_of_Array_Index"
write_c "${DIR_129}" "129.c" "/* CWE-129 */
volatile int sink;
void start(void) {
    int arr[8];
    int idx = 20;  /* unchecked index */
    arr[idx] = 42; /* out-of-bounds write */
    sink = arr[0];
}
"

###############################################################################
# 131 – Incorrect Calculation of Buffer Size
###############################################################################
DIR_131="${SRC_ROOT}/131_Incorrect_Calculation_of_Buffer_Size"
write_c "${DIR_131}" "131.c" "/* CWE-131 */
volatile int sink;
void start(void) {
    int arr[8];
    int *p = arr;
    int wrong_size = sizeof(p);  /* sizeof pointer, not array */
    for (int i = 0; i < wrong_size; i++) {
        arr[i] = i;  /* may exceed arr[7] */
    }
    sink = arr[0];
}
"

###############################################################################
# 134 – Use of Externally-Controlled Format String (simulated)
###############################################################################
DIR_134="${SRC_ROOT}/134_Use_of_Externally_Controlled_Format_String"
write_c "${DIR_134}" "134.c" "/* CWE-134 (simulated mini printf) */
volatile int sink;
void my_printf(const char *fmt) {
    int acc = 0;
    for (int i = 0; ; i++) {
        char c = fmt[i];
        if (!c) break;
        if (c == '%') {
            char next = fmt[i+4];  /* over-read using 'specifier' */
            acc += (int)next;
        } else {
            acc += (int)c;
        }
    }
    sink = acc;
}
void start(void) {
    char user_buf[16];
    for (int i = 0; i < 16; i++) user_buf[i] = (i % 4 == 0) ? '%' : 'A';
    my_printf(user_buf);
}
"

###############################################################################
# 188 – Reliance on Data/Memory Layout
###############################################################################
DIR_188="${SRC_ROOT}/188_Reliance_on_Data_or_Memory_Layout"
write_c "${DIR_188}" "188.c" "/* CWE-188 */
volatile int sink;
struct S { char a; int b; char c; };
void start(void) {
    struct S s;
    s.a = 1;
    s.b = 0x22223333;
    s.c = 3;
    char *p = (char *)&s.a;
    int *pb = (int *)(p + 1); /* assumes no padding */
    sink = *pb;
}
"

###############################################################################
# 198 – Use of Incorrect Byte Ordering
###############################################################################
DIR_198="${SRC_ROOT}/198_Use_of_Incorrect_Byte_Ordering"
write_c "${DIR_198}" "198.c" "/* CWE-198 */
volatile int sink;
void start(void) {
    unsigned char bytes[4];
    bytes[0] = 1;
    bytes[1] = 2;
    bytes[2] = 3;
    bytes[3] = 4;
    /* Misinterpreted endianness */
    unsigned int v = ((unsigned int)bytes[0] << 24) |
                     ((unsigned int)bytes[1] << 16) |
                     ((unsigned int)bytes[2] << 8)  |
                     ((unsigned int)bytes[3]);
    sink = (int)v;
}
"

###############################################################################
# 244 – Improper Clearing of Heap Memory Before Release
###############################################################################
DIR_244="${SRC_ROOT}/244_Improper_Clearing_of_Heap_Memory_Before_Release"
write_c "${DIR_244}" "244.c" "/* CWE-244 */
volatile int sink;
static char heap_area[32];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(char *p, int n) {
    for (int i = 0; i < n - 4; i++) { /* tails not cleared */
        p[i] = 0;
    }
}
void start(void) {
    char *p = my_alloc(16);
    if (!p) return;
    for (int i = 0; i < 16; i++) p[i] = (char)(0x60 + i);
    my_free(p, 16);
    sink = p[15];  /* may still hold 'secret' */
}
"

###############################################################################
# 401 – Missing Release of Memory after Effective Lifetime
###############################################################################
DIR_401="${SRC_ROOT}/401_Missing_Release_of_Memory"
write_c "${DIR_401}" "401.c" "/* CWE-401 */
static char heap_area[128];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 128) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void start(void) {
    for (int r = 0; r < 16; r++) {
        (void)my_alloc(8); /* never freed */
    }
}
"

###############################################################################
# 415 – Double Free
###############################################################################
DIR_415="${SRC_ROOT}/415_Double_Free"
write_c "${DIR_415}" "415.c" "/* CWE-415 */
static char heap_area[32];
static int heap_used = 0;
static int freed = 0;

void *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    freed = 0;
    return p;
}
void my_free(void *p) {
    if (freed) {
        heap_used += 4; /* corrupt heap state */
    }
    freed = 1;
}
void start(void) {
    void *p = my_alloc(8);
    if (!p) return;
    my_free(p);
    my_free(p);  /* double free */
}
"

###############################################################################
# 416 – Use After Free
###############################################################################
DIR_416="${SRC_ROOT}/416_Use_After_Free"
write_c "${DIR_416}" "416.c" "/* CWE-416 */
volatile int sink;
static char heap_area[32];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(char *p, int n) {
    for (int i = 0; i < n; i++) p[i] = (char)0xAA;
}
void start(void) {
    char *p = my_alloc(16);
    if (!p) return;
    for (int i = 0; i < 16; i++) p[i] = (char)i;
    my_free(p, 16);
    p[3] = 7; /* UAF */
    sink = p[0];
}
"

###############################################################################
# 466 – Return of Pointer Value Outside of Expected Range
###############################################################################
DIR_466="${SRC_ROOT}/466_Return_of_Pointer_Value_Outside_of_Expected_Range"
write_c "${DIR_466}" "466.c" "/* CWE-466 */
volatile int sink;
static int data[8];

int *get_ptr(void) {
    return &data[16];  /* pointer outside array */
}
void start(void) {
    int *p = get_ptr();
    *p = 42;
    sink = data[0];
}
"

###############################################################################
# 562 – Return of Stack Variable Address
###############################################################################
DIR_562="${SRC_ROOT}/562_Return_of_Stack_Variable_Address"
write_c "${DIR_562}" "562.c" "/* CWE-562 */
volatile int sink;
int *get_local(void) {
    int local[4];
    local[1] = 10;
    return &local[1];  /* returns stack address */
}
void start(void) {
    int *p = get_local();
    sink = *p;  /* invalid dereference */
}
"

###############################################################################
# 587 – Assignment of a Fixed Address to a Pointer
###############################################################################
DIR_587="${SRC_ROOT}/587_Assignment_of_a_Fixed_Address_to_a_Pointer"
write_c "${DIR_587}" "587.c" "/* CWE-587 */
volatile int sink;
void start(void) {
    int *p = (int *)0x1000u;  /* fixed address */
    *p = 123;
    sink = *p;
}
"

###############################################################################
# 590 – Free of Memory not on the Heap
###############################################################################
DIR_590="${SRC_ROOT}/590_Free_of_Memory_not_on_the_Heap"
write_c "${DIR_590}" "590.c" "/* CWE-590 */
static char heap_area[32];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(void *p) {
    if (p < (void *)heap_area || p > (void *)(heap_area + 32)) {
        heap_used += 8; /* corrupt heap on non-heap pointer */
    }
}
void start(void) {
    char stack_buf[8];
    my_free((void *)stack_buf);  /* not allocated from heap */
}
"

###############################################################################
# 680 – Integer Overflow to Buffer Overflow
###############################################################################
DIR_680="${SRC_ROOT}/680_Integer_Overflow_to_Buffer_Overflow"
write_c "${DIR_680}" "680.c" "/* CWE-680 */
volatile int sink;
void start(void) {
    unsigned int count = 0x40000000u;
    unsigned int size = count * 8;  /* overflow */
    char buf[32];
    for (unsigned int i = 0; i < size; i++) {
        buf[i] = (char)i;  /* overflow buffer */
    }
    sink = buf[0];
}
"

###############################################################################
# 690 – Unchecked Return Value to NULL Pointer Dereference
###############################################################################
DIR_690="${SRC_ROOT}/690_Unchecked_Return_Value_to_NULL_Pointer_Dereference"
write_c "${DIR_690}" "690.c" "/* CWE-690 */
volatile int sink;
static char heap_area[16];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 16) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void start(void) {
    char *p = my_alloc(32); /* will return NULL */
    p[0] = 1;               /* unchecked deref */
    sink = p[0];
}
"

###############################################################################
# 761 – Free of Pointer not at Start of Buffer
###############################################################################
DIR_761="${SRC_ROOT}/761_Free_of_Pointer_not_at_Start_of_Buffer"
write_c "${DIR_761}" "761.c" "/* CWE-761 */
static char heap_area[32];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(void *p) {
    if (p != (void *)heap_area) {
        heap_used += 4;   /* mis-handles interior pointer */
    }
}
void start(void) {
    char *base = my_alloc(16);
    if (!base) return;
    char *p = base + 4;
    my_free(p);  /* not the start of buffer */
}
"

###############################################################################
# 762 – Mismatched Memory Management Routines
###############################################################################
DIR_762="${SRC_ROOT}/762_Mismatched_Memory_Management_Routines"
write_c "${DIR_762}" "762.c" "/* CWE-762 */
static char heap_A[32];
static char heap_B[32];
static int used_A = 0;
static int used_B = 0;

void *alloc_A(int n) {
    if (used_A + n > 32) return 0;
    void *p = &heap_A[used_A];
    used_A += n;
    return p;
}
void free_B(void *p) {
    if (p >= (void *)heap_B && p < (void *)(heap_B + 32)) {
        used_B -= 4;
    }
}
void start(void) {
    void *p = alloc_A(8);
    if (!p) return;
    free_B(p);  /* mismatched allocator and deallocator */
}
"

###############################################################################
# 763 – Release of Invalid Pointer or Reference
###############################################################################
DIR_763="${SRC_ROOT}/763_Release_of_Invalid_Pointer_or_Reference"
write_c "${DIR_763}" "763.c" "/* CWE-763 */
static char heap_area[32];
static int heap_used = 0;

void *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    void *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(void *p) {
    if (p < (void *)heap_area || p > (void *)(heap_area + 32)) {
        heap_used += 4; /* corrupt state when given invalid pointer */
    }
}
void start(void) {
    char bogus[8];
    my_free((void *)(bogus + 2));  /* not from heap */
}
"

###############################################################################
# 786 – Access of Memory Location Before Start of Buffer
###############################################################################
DIR_786="${SRC_ROOT}/786_Access_of_Memory_Before_Start_of_Buffer"
write_c "${DIR_786}" "786.c" "/* CWE-786 */
volatile int sink;
void start(void) {
    int buf[8];
    int *p = &buf[0];
    p -= 2;         /* move before buffer */
    *p = 123;       /* write before start */
    sink = buf[0];
}
"

###############################################################################
# 787 – Out-of-bounds Write
###############################################################################
DIR_787="${SRC_ROOT}/787_Out_of_bounds_Write"
write_c "${DIR_787}" "787.c" "/* CWE-787 */
volatile int sink;
void start(void) {
    int buf[8];
    for (int i = 0; i < 16; i++) {
        buf[i] = i;   /* write beyond buf[7] */
    }
    sink = buf[0];
}
"

###############################################################################
# 788 – Access of Memory Location After End of Buffer
###############################################################################
DIR_788="${SRC_ROOT}/788_Access_of_Memory_After_End_of_Buffer"
write_c "${DIR_788}" "788.c" "/* CWE-788 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (char)i;
    char v = buf[12];   /* access after end */
    sink = (int)v;
}
"

###############################################################################
# 789 – Memory Allocation with Excessive Size Value
###############################################################################
DIR_789="${SRC_ROOT}/789_Memory_Allocation_with_Excessive_Size_Value"
write_c "${DIR_789}" "789.c" "/* CWE-789 */
volatile int sink;
static char heap_area[64];
static int heap_used = 0;

char *my_alloc(unsigned int n) {
    if (n > 1000u) return 0;  /* weak limit */
    if (heap_used + (int)n > 64) return 0;
    char *p = &heap_area[heap_used];
    heap_used += (int)n;
    return p;
}
void start(void) {
    unsigned int huge = 512u;
    char *p = my_alloc(huge);  /* logically excessive */
    if (!p) return;
    p[0] = 1;
    sink = p[0];
}
"

###############################################################################
# 805 – Buffer Access with Incorrect Length Value
###############################################################################
DIR_805="${SRC_ROOT}/805_Buffer_Access_with_Incorrect_Length_Value"
write_c "${DIR_805}" "805.c" "/* CWE-805 */
volatile int sink;
void start(void) {
    char buf[8];
    int len = 12; /* incorrect length */
    for (int i = 0; i < len; i++) {
        buf[i] = (char)i;   /* overflow */
    }
    sink = buf[0];
}
"

###############################################################################
# 806 – Buffer Access Using Size of Source Buffer
###############################################################################
DIR_806="${SRC_ROOT}/806_Buffer_Access_Using_Size_of_Source_Buffer"
write_c "${DIR_806}" "806.c" "/* CWE-806 */
volatile int sink;
void start(void) {
    char src[16];
    char dst[8];
    for (int i = 0; i < 16; i++) src[i] = (char)i;
    int len = sizeof(src);  /* uses source size */
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];   /* overflow dst */
    }
    sink = dst[0];
}
"

###############################################################################
# 822 – Untrusted Pointer Dereference
###############################################################################
DIR_822="${SRC_ROOT}/822_Untrusted_Pointer_Dereference"
write_c "${DIR_822}" "822.c" "/* CWE-822 */
volatile int sink;
volatile unsigned long untrusted_addr = 0x2000u;
void start(void) {
    int *p = (int *)untrusted_addr;
    *p = 42;  /* dereference untrusted pointer */
    sink = *p;
}
"

###############################################################################
# 823 – Use of Out-of-range Pointer Offset
###############################################################################
DIR_823="${SRC_ROOT}/823_Use_of_Out_of_range_Pointer_Offset"
write_c "${DIR_823}" "823.c" "/* CWE-823 */
volatile int sink;
void start(void) {
    int buf[16];
    int *p = &buf[0];
    int *q = p + 32;  /* out-of-range offset */
    *q = 99;
    sink = buf[0];
}
"

###############################################################################
# 824 – Access of Uninitialized Pointer
###############################################################################
DIR_824="${SRC_ROOT}/824_Access_of_Uninitialized_Pointer"
write_c "${DIR_824}" "824.c" "/* CWE-824 */
volatile int sink;
void start(void) {
    int *p;
    *p = 42;  /* use of uninitialized pointer */
    sink = *p;
}
"

###############################################################################
# 825 – Expired Pointer Dereference
###############################################################################
DIR_825="${SRC_ROOT}/825_Expired_Pointer_Dereference"
write_c "${DIR_825}" "825.c" "/* CWE-825 */
volatile int sink;
static char heap_area[32];
static int heap_used = 0;

char *my_alloc(int n) {
    if (heap_used + n > 32) return 0;
    char *p = &heap_area[heap_used];
    heap_used += n;
    return p;
}
void my_free(char *p, int n) {
    for (int i = 0; i < n; i++) p[i] = (char)0xAA;
}
void start(void) {
    char *p = my_alloc(16);
    if (!p) return;
    for (int i = 0; i < 16; i++) p[i] = (char)i;
    my_free(p, 16);
    p[4] = 7;  /* expired pointer dereference */
    sink = p[0];
}
"

echo "One benchmark per CWE generated under: ${SRC_ROOT}"
