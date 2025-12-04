#!/usr/bin/env bash
set -euo pipefail

# Initialize all CWE-style memory safety benchmarks in c/
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
C_DIR="$ROOT_DIR/c"

mkdir -p "$C_DIR"

write_bench() {
  local fname="$1"
  shift
  cat > "$C_DIR/$fname.c" << 'EOF'
/* Auto-generated benchmark */

volatile int sink;

void touch(volatile char *p) {
    sink += *p;
}

void start(void) {
    /* BODY */
}
EOF
}

###############################################################################
# For each file, we rewrite the body in-place after creating the template
###############################################################################

# 119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer
cat > "$C_DIR/119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer.c" << 'EOF'
/* CWE 119: Improper Restriction of Operations within the Bounds of a Memory Buffer */

volatile int sink;

void start(void) {
    char buf[16];
    char *p = buf;
    int i;

    /* Writes beyond the logical object limit (improper bounds use) */
    for (i = 0; i < 32; i++) {
        p[i] = (char)i; /* out-of-bounds for i >= 16 */
    }

    sink = p[0];
}
EOF

# 120_Buffer_Copy_without_Checking_Size_of_Input
cat > "$C_DIR/120_Buffer_Copy_without_Checking_Size_of_Input.c" << 'EOF'
/* CWE 120: Buffer Copy without Checking Size of Input */

volatile int sink;

void start(void) {
    char src[64];
    char dst[16];
    int i;

    for (i = 0; i < 64; i++) {
        src[i] = (char)i;
    }

    /* Copy more bytes than destination can hold */
    for (i = 0; i < 64; i++) {
        dst[i] = src[i]; /* out-of-bounds for i >= 16 */
    }

    sink = dst[0];
}
EOF

# 121_Stack_based_Buffer_Overflow
cat > "$C_DIR/121_Stack_based_Buffer_Overflow.c" << 'EOF'
/* CWE 121: Stack-based Buffer Overflow */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i <= 8; i++) {
        buf[i] = (char)(i + 1); /* write at buf[8] is out-of-bounds */
    }

    sink = buf[0];
}
EOF

# 122_Heap_based_Buffer_Overflow (simulate heap with static buffer)
cat > "$C_DIR/122_Heap_based_Buffer_Overflow.c" << 'EOF'
/* CWE 122: Heap-based Buffer Overflow (simulated heap) */

volatile int sink;
static char heap_area[32];

void start(void) {
    char *heap_buf = heap_area; /* pretend this came from an allocator */
    int i;

    for (i = 0; i < 64; i++) {
        heap_buf[i] = (char)i; /* out-of-bounds for i >= 32 */
    }

    sink = heap_buf[0];
}
EOF

# 123_Write_what_where_Condition
cat > "$C_DIR/123_Write_what_where_Condition.c" << 'EOF'
/* CWE 123: Write-what-where Condition */

volatile int sink;

void start(void) {
    int target = 1;
    int other  = 2;

    int *where = &target;
    int *p = &other;

    /* Corrupt "where" via invalid offset computation */
    where = (int *)((char *)where + sizeof(int) * 2); /* points beyond */

    /* Now write to unintended location */
    *where = *p;

    sink = target;
}
EOF

# 124_Buffer_Underwrite
cat > "$C_DIR/124_Buffer_Underwrite.c" << 'EOF'
/* CWE 124: Buffer Underwrite (Buffer Underflow) */

volatile int sink;

void start(void) {
    char buf[16];
    char *p = buf + 4;
    int i;

    /* Write before the start of the buffer */
    for (i = -4; i < 8; i++) {
        p[i] = (char)i; /* p[-4]..p[-1] underflow buf */
    }

    sink = buf[0];
}
EOF

# 125_Out_of_bounds_Read
cat > "$C_DIR/125_Out_of_bounds_Read.c" << 'EOF'
/* CWE 125: Out-of-bounds Read */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(10 + i);
    }

    /* Read past the end */
    for (i = 0; i < 16; i++) {
        sink += buf[i]; /* buf[8..15] are out-of-bounds */
    }
}
EOF

# 126_Buffer_Over_read
cat > "$C_DIR/126_Buffer_Over_read.c" << 'EOF'
/* CWE 126: Buffer Over-read */

volatile int sink;

void start(void) {
    char buf[4];
    int i;

    for (i = 0; i < 4; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = 0; i < 10; i++) {
        sink += buf[i]; /* buf[4..9] are out-of-bounds */
    }
}
EOF

# 127_Buffer_Under_read
cat > "$C_DIR/127_Buffer_Under_read.c" << 'EOF'
/* CWE 127: Buffer Under-read */

volatile int sink;

void start(void) {
    char buf[8];
    char *p = buf + 4;
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(i + 10);
    }

    for (i = -4; i < 4; i++) {
        sink += p[i]; /* p[-4..-1] read before buf */
    }
}
EOF

# 129_Improper_Validation_of_Array_Index
cat > "$C_DIR/129_Improper_Validation_of_Array_Index.c" << 'EOF'
/* CWE 129: Improper Validation of Array Index */

volatile int sink;

void start(void) {
    int arr[8];
    int idx = 16; /* unvalidated external value (simulated) */

    arr[0] = 1;

    /* use index without checking range */
    arr[idx] = 42; /* out-of-bounds write */

    sink = arr[0];
}
EOF

# 131_Incorrect_Calculation_of_Buffer_Size
cat > "$C_DIR/131_Incorrect_Calculation_of_Buffer_Size.c" << 'EOF'
/* CWE 131: Incorrect Calculation of Buffer Size */

volatile int sink;

void start(void) {
    char buf[10];
    int count = 10;
    int i;

    /* Intended to allocate 10 bytes but uses 10 elements of 2 bytes each conceptually */
    for (i = 0; i < count * 2; i++) {
        buf[i] = (char)i; /* buf[10..19] out-of-bounds */
    }

    sink = buf[0];
}
EOF

# 134_Use_of_Externally_Controlled_Format_String (logic only)
cat > "$C_DIR/134_Use_of_Externally_Controlled_Format_String.c" << 'EOF'
/* CWE 134: Use of Externally-Controlled Format String (simulated, no I/O) */

volatile int sink;

void start(void) {
    char fmt[16];
    char user[32];
    int i;

    /* simulate attacker-controlled string with % specifiers */
    for (i = 0; i < 16; i++) {
        fmt[i] = '%';
    }
    for (i = 0; i < 32; i++) {
        user[i] = (char)(i + 33);
    }

    /* No real printf here due to nostdlib; treat as misuse of untrusted layout */
    sink = fmt[0] + user[0];
}
EOF

# 188_Reliance_on_Data_Memory_Layout
cat > "$C_DIR/188_Reliance_on_Data_Memory_Layout.c" << 'EOF'
/* CWE 188: Reliance on Data/Memory Layout */

volatile int sink;

struct Pair {
    int x;
    int y;
};

void start(void) {
    struct Pair p;
    int *raw = (int *)&p;

    raw[0] = 1;
    raw[1] = 2;

    /* Assume that "y" immediately follows "x" and manipulate via index */
    raw[2] = 3; /* out-of-struct write */

    sink = p.x + p.y;
}
EOF

# 198_Use_of_Incorrect_Byte_Ordering
cat > "$C_DIR/198_Use_of_Incorrect_Byte_Ordering.c" << 'EOF'
/* CWE 198: Use of Incorrect Byte Ordering */

volatile int sink;

void start(void) {
    unsigned char buf[4];
    unsigned int value = 0x01020304u;
    unsigned int reconstructed;
    int i;

    /* Naive store as big-endian */
    buf[0] = (unsigned char)((value >> 24) & 0xffu);
    buf[1] = (unsigned char)((value >> 16) & 0xffu);
    buf[2] = (unsigned char)((value >> 8)  & 0xffu);
    buf[3] = (unsigned char)(value & 0xffu);

    /* Naive load as little-endian */
    reconstructed = 0;
    for (i = 0; i < 4; i++) {
        reconstructed |= ((unsigned int)buf[i]) << (8 * i);
    }

    sink = (int)reconstructed;
}
EOF

# 244_Improper_Clearing_of_Heap_Memory_Before_Release
cat > "$C_DIR/244_Improper_Clearing_of_Heap_Memory_Before_Release.c" << 'EOF'
/* CWE 244: Improper Clearing of Heap Memory Before Release (simulated) */

volatile int sink;
static char heap_area[32];

void fake_free(char *p, int n) {
    int i;
    /* failure to clear sensitive data; here, we just leave it */
    for (i = 0; i < n; i++) {
        sink += p[i]; /* "leak" via side-channel sink */
    }
}

void start(void) {
    char *p = heap_area;
    int i;

    for (i = 0; i < 32; i++) {
        p[i] = (char)(i + 1); /* pretend secret data */
    }

    fake_free(p, 32);
}
EOF

# 401_Missing_Release_of_Memory_after_Effective_Lifetime
cat > "$C_DIR/401_Missing_Release_of_Memory_after_Effective_Lifetime.c" << 'EOF'
/* CWE 401: Missing Release of Memory after Effective Lifetime (leak) */

volatile int sink;
static char heap_area[256];
static int heap_index;

char *alloc_block(int n) {
    char *p;
    if (heap_index + n > 256) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void start(void) {
    char *blocks[8];
    int i, j;

    heap_index = 0;

    for (i = 0; i < 8; i++) {
        blocks[i] = alloc_block(24);
        if (!blocks[i]) {
            break;
        }
        for (j = 0; j < 24; j++) {
            blocks[i][j] = (char)(i + j);
        }
    }

    /* never free blocks -> leak within artificial heap */
    for (i = 0; i < 8; i++) {
        if (blocks[i]) {
            sink += blocks[i][0];
        }
    }
}
EOF

# 415_Double_Free (simulated)
cat > "$C_DIR/415_Double_Free.c" << 'EOF'
/* CWE 415: Double Free (simulated custom heap) */

volatile int sink;
static char heap_area[64];
static int heap_index;
static int freed;

char *my_alloc(int n) {
    char *p;
    if (heap_index + n > 64) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void my_free(char *p) {
    /* simulate double-free detection via simple flag misuse */
    if (freed) {
        /* second free -> memory safety misuse */
        sink += 1;
    }
    freed = 1;
}

void start(void) {
    char *p = my_alloc(16);
    int i;

    freed = 0;

    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }

    my_free(p);
    my_free(p); /* double free */
}
EOF

# 416_Use_After_Free (simulated)
cat > "$C_DIR/416_Use_After_Free.c" << 'EOF'
/* CWE 416: Use After Free (simulated custom heap) */

volatile int sink;
static char heap_area[64];
static int heap_index;

char *my_alloc(int n) {
    char *p;
    if (heap_index + n > 64) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void my_free(char *p) {
    int i;
    /* "free" by scribbling metadata */
    for (i = 0; i < 16; i++) {
        p[i] = 0;
    }
}

void start(void) {
    char *p = my_alloc(16);
    int i;

    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }

    my_free(p);

    /* use-after-free: still dereference p */
    for (i = 0; i < 16; i++) {
        sink += p[i];
    }
}
EOF

# 466_Return_of_Pointer_Value_Outside_of_Expected_Range
cat > "$C_DIR/466_Return_of_Pointer_Value_Outside_of_Expected_Range.c" << 'EOF'
/* CWE 466: Return of Pointer Value Outside of Expected Range */

volatile int sink;

char *get_buffer(void) {
    static char buf[16];
    /* incorrect: return pointer to middle, not to start */
    return buf + 8;
}

void start(void) {
    char *p = get_buffer();
    int i;

    for (i = -8; i < 8; i++) {
        p[i] = (char)i; /* p[-8].. underflow the true object start */
    }

    sink = p[0];
}
EOF

# 562_Return_of_Stack_Variable_Address
cat > "$C_DIR/562_Return_of_Stack_Variable_Address.c" << 'EOF'
/* CWE 562: Return of Stack Variable Address */

volatile int sink;

char *get_stack_buffer(void) {
    char buf[16];
    int i;
    for (i = 0; i < 16; i++) {
        buf[i] = (char)(i + 1);
    }
    return buf; /* returning stack address */
}

void start(void) {
    char *dangling = get_stack_buffer();
    int i;

    for (i = 0; i < 16; i++) {
        sink += dangling[i]; /* use of invalid stack pointer */
    }
}
EOF

# 587_Assignment_of_a_Fixed_Address_to_a_Pointer
cat > "$C_DIR/587_Assignment_of_a_Fixed_Address_to_a_Pointer.c" << 'EOF'
/* CWE 587: Assignment of a Fixed Address to a Pointer */

volatile int sink;

void start(void) {
    /* arbitrary fixed address, invalid in normal execution */
    int *p = (int *)0x100u;

    /* dereference invalid pointer */
    *p = 42;

    sink = *p;
}
EOF

# 590_Free_of_Memory_not_on_the_Heap
cat > "$C_DIR/590_Free_of_Memory_not_on_the_Heap.c" << 'EOF'
/* CWE 590: Free of Memory not on the Heap (simulated) */

volatile int sink;

void my_free(char *p) {
    /* pretend this only accepts heap pointers; we misuse with stack */
    sink += (int)(unsigned long)(p != 0);
}

void start(void) {
    char buf[16];
    my_free(buf); /* freeing stack memory */
}
EOF

# 680_Integer_Overflow_to_Buffer_Overflow
cat > "$C_DIR/680_Integer_Overflow_to_Buffer_Overflow.c" << 'EOF'
/* CWE 680: Integer Overflow to Buffer Overflow */

volatile int sink;
static char heap_area[64];
static int heap_index;

char *my_alloc(int n) {
    char *p;
    if (heap_index + n > 64) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void start(void) {
    unsigned int count = 0x80000001u; /* huge count */
    unsigned int size_per_elem = 4u;
    unsigned int total = count * size_per_elem; /* overflow */

    char *p = my_alloc((int)total);
    int i;

    if (!p) {
        /* fallback to some region anyway */
        p = heap_area;
    }

    for (i = 0; i < 64; i++) {
        p[i] = (char)i; /* may overflow intended region */
    }

    sink = p[0];
}
EOF

# 690_Unchecked_Return_Value_to_NULL_Pointer_Dereference
cat > "$C_DIR/690_Unchecked_Return_Value_to_NULL_Pointer_Dereference.c" << 'EOF'
/* CWE 690: Unchecked Return Value to NULL Pointer Dereference */

volatile int sink;
static char heap_area[32];
static int heap_index;

char *may_fail_alloc(int n) {
    if (heap_index + n > 32) {
        return 0;
    }
    heap_index += n;
    return &heap_area[heap_index - n];
}

void start(void) {
    char *p;

    heap_index = 0;

    p = may_fail_alloc(40); /* fails, returns NULL */
    /* unchecked */
    p[0] = 1; /* NULL deref */
    sink = p[0];
}
EOF

# 761_Free_of_Pointer_not_at_Start_of_Buffer
cat > "$C_DIR/761_Free_of_Pointer_not_at_Start_of_Buffer.c" << 'EOF'
/* CWE 761: Free of Pointer not at Start of Buffer (simulated) */

volatile int sink;

void my_free(char *p) {
    /* expect pointer to start of buffer; using interior pointer is bad */
    sink += (int)(unsigned long)p;
}

void start(void) {
    char buf[16];
    char *mid = buf + 4;
    my_free(mid); /* not at start */
}
EOF

# 762_Mismatched_Memory_Management_Routines (simulated)
cat > "$C_DIR/762_Mismatched_Memory_Management_Routines.c" << 'EOF'
/* CWE 762: Mismatched Memory Management Routines (simulated) */

volatile int sink;
static char heap_area[32];

void my_free(char *p) {
    sink += (int)(unsigned long)p;
}

void start(void) {
    char *p = heap_area; /* pretend this came from a different allocator */
    my_free(p); /* mismatched "free" */
}
EOF

# 763_Release_of_Invalid_Pointer_or_Reference
cat > "$C_DIR/763_Release_of_Invalid_Pointer_or_Reference.c" << 'EOF'
/* CWE 763: Release of Invalid Pointer or Reference (simulated) */

volatile int sink;

void my_free(char *p) {
    sink += (int)(unsigned long)p;
}

void start(void) {
    char *invalid = (char *)0x4u;
    my_free(invalid);
}
EOF

# 786_Access_of_Memory_Location_Before_Start_of_Buffer
cat > "$C_DIR/786_Access_of_Memory_Location_Before_Start_of_Buffer.c" << 'EOF'
/* CWE 786: Access of Memory Location Before Start of Buffer */

volatile int sink;

void start(void) {
    char buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = -4; i < 0; i++) {
        sink += buf[i]; /* out-of-bounds before start */
    }
}
EOF

# 787_Out_of_bounds_Write
cat > "$C_DIR/787_Out_of_bounds_Write.c" << 'EOF'
/* CWE 787: Out-of-bounds Write */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i < 16; i++) {
        buf[i] = (char)i; /* buf[8..15] out-of-bounds */
    }

    sink = buf[0];
}
EOF

# 788_Access_of_Memory_Location_After_End_of_Buffer
cat > "$C_DIR/788_Access_of_Memory_Location_After_End_of_Buffer.c" << 'EOF'
/* CWE 788: Access of Memory Location After End of Buffer */

volatile int sink;

void start(void) {
    char buf[8];
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = 8; i < 16; i++) {
        sink += buf[i]; /* accesses past end */
    }
}
EOF

# 789_Memory_Allocation_with_Excessive_Size_Value (simulated)
cat > "$C_DIR/789_Memory_Allocation_with_Excessive_Size_Value.c" << 'EOF'
/* CWE 789: Memory Allocation with Excessive Size Value (simulated) */

volatile int sink;
static char heap_area[64];

char *my_alloc(int n) {
    if (n > 64) {
        return 0;
    }
    return heap_area;
}

void start(void) {
    char *p = my_alloc(1000); /* excessive request */
    int i;

    if (!p) {
        /* still attempt to use it as if allocation succeeded */
        p = (char *)0;
    }

    for (i = 0; i < 16; i++) {
        p[i] = (char)i; /* may deref NULL */
    }

    sink = p[0];
}
EOF

# 805_Buffer_Access_with_Incorrect_Length_Value
cat > "$C_DIR/805_Buffer_Access_with_Incorrect_Length_Value.c" << 'EOF'
/* CWE 805: Buffer Access with Incorrect Length Value */

volatile int sink;

void start(void) {
    char buf[8];
    int length = 16; /* incorrect length */
    int i;

    for (i = 0; i < 8; i++) {
        buf[i] = (char)(i + 1);
    }

    for (i = 0; i < length; i++) {
        sink += buf[i]; /* out-of-bounds for i >= 8 */
    }
}
EOF

# 806_Buffer_Access_Using_Size_of_Source_Buffer
cat > "$C_DIR/806_Buffer_Access_Using_Size_of_Source_Buffer.c" << 'EOF'
/* CWE 806: Buffer Access Using Size of Source Buffer */

volatile int sink;

void start(void) {
    char src[16];
    char dst[4];
    int i;

    for (i = 0; i < 16; i++) {
        src[i] = (char)(i + 1);
    }

    /* use size of src instead of size of dst */
    for (i = 0; i < 16; i++) {
        dst[i] = src[i]; /* dst[4..15] out-of-bounds */
    }

    sink = dst[0];
}
EOF

# 822_Untrusted_Pointer_Dereference
cat > "$C_DIR/822_Untrusted_Pointer_Dereference.c" << 'EOF'
/* CWE 822: Untrusted Pointer Dereference */

volatile int sink;

void start(void) {
    unsigned long attacker_value = 0x8u;
    int *p = (int *)attacker_value; /* untrusted pointer */

    *p = 123; /* arbitrary write via untrusted pointer */

    sink = *p;
}
EOF

# 823_Use_of_Out_of_range_Pointer_Offset
cat > "$C_DIR/823_Use_of_Out_of_range_Pointer_Offset.c" << 'EOF'
/* CWE 823: Use of Out-of-range Pointer Offset */

volatile int sink;

void start(void) {
    int arr[4];
    int *p = arr;
    int *q = p + 10; /* out-of-range offset */

    *q = 5; /* write beyond array */

    sink = arr[0];
}
EOF

# 824_Access_of_Uninitialized_Pointer
cat > "$C_DIR/824_Access_of_Uninitialized_Pointer.c" << 'EOF'
/* CWE 824: Access of Uninitialized Pointer */

volatile int sink;

void start(void) {
    int *p; /* uninitialized pointer */

    *p = 42; /* undefined behavior */

    sink = *p;
}
EOF

# 825_Expired_Pointer_Dereference
cat > "$C_DIR/825_Expired_Pointer_Dereference.c" << 'EOF'
/* CWE 825: Expired Pointer Dereference */

volatile int sink;
static char heap_area[32];
static int heap_index;

char *my_alloc(int n) {
    char *p;
    if (heap_index + n > 32) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void my_reset_heap(void) {
    heap_index = 0;
}

void start(void) {
    char *p;
    int i;

    my_reset_heap();
    p = my_alloc(16);

    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }

    my_reset_heap(); /* invalidates previous allocation logically */

    /* expired pointer dereference */
    for (i = 0; i < 16; i++) {
        sink += p[i];
    }
}
EOF

echo "Initialized CWE benchmark C files in: $C_DIR"
