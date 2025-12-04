#!/usr/bin/env bash
set -euo pipefail

# Directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"
SRC_DIR="${ROOT_DIR}/c"

mkdir -p "${SRC_DIR}"

echo "Initializing CWE memory safety benchmarks in: ${SRC_DIR}"

########################################
# 119 – Improper Restriction of Operations within the Bounds of a Memory Buffer
########################################
cat > "${SRC_DIR}/119_Improper_Restriction_of_Operations_within_the_Bounds_of_a_Memory_Buffer.c" << 'EOF'
/* CWE 119: Improper Restriction of Operations within the Bounds of a Memory Buffer */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    int len = 32; /* incorrect bound */
    char *p = buf;

    for (i = 0; i < len; i++) {
        p[i] = (char)i; /* writes past buf[15] */
    }

    sink = p[0];
}
EOF

########################################
# 120 – Classic Buffer Overflow
########################################
cat > "${SRC_DIR}/120_Buffer_Copy_without_Checking_Size_of_Input_Classic_Buffer_Overflow.c" << 'EOF'
/* CWE 120: Buffer Copy without Checking Size of Input (Classic Buffer Overflow) */
static char dst[16];
static char src[32];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 32; i++) {
        src[i] = (char)i;
    }

    /* naive copy: assumes dst can hold 32 bytes */
    for (i = 0; i < 32; i++) {
        dst[i] = src[i]; /* overflow dst */
    }

    sink = dst[0];
}
EOF

########################################
# 121 – Stack based Buffer Overflow
########################################
cat > "${SRC_DIR}/121_Stack_based_Buffer_Overflow.c" << 'EOF'
/* CWE 121: Stack-based Buffer Overflow */
volatile int sink;

void start(void) {
    char local[8];
    int i;

    for (i = 0; i < 24; i++) {
        local[i] = (char)i; /* overflow stack buffer */
    }

    sink = local[0];
}
EOF

########################################
# 122 – Heap based Buffer Overflow (simulated heap)
########################################
cat > "${SRC_DIR}/122_Heap_based_Buffer_Overflow.c" << 'EOF'
/* CWE 122: Heap-based Buffer Overflow (simulated tiny heap) */
static char fake_heap[32];
volatile int sink;

char *my_alloc(int n) {
    if (n > 32) {
        return 0;
    }
    return fake_heap;
}

void start(void) {
    char *p = my_alloc(16);
    int i;

    if (!p) {
        return;
    }

    for (i = 0; i < 40; i++) {
        p[i] = (char)i; /* overflow fake_heap */
    }

    sink = p[0];
}
EOF

########################################
# 123 – Write what where Condition
########################################
cat > "${SRC_DIR}/123_Write_what_where_Condition.c" << 'EOF'
/* CWE 123: Write-what-where Condition */
static int target1 = 1;
static int target2 = 2;
volatile int sink;

void start(void) {
    int value = 42;
    int *where;

    /* badly validated index controlling where we write */
    int idx = 1; /* attacker-controlled in real world */
    if (idx == 0) {
        where = &target1;
    } else {
        where = &target2;
    }

    *where = value; /* arbitrary write target */
    sink = target1 + target2;
}
EOF

########################################
# 124 – Buffer Underwrite
########################################
cat > "${SRC_DIR}/124_Buffer_Underwrite_Buffer_Underflow.c" << 'EOF'
/* CWE 124: Buffer Underwrite (Buffer Underflow) */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    char *p = buf + 4;

    for (i = 0; i < 16; i++) {
        p[-i] = (char)i; /* writes before buf */
    }

    sink = buf[0];
}
EOF

########################################
# 125 – Out of bounds Read
########################################
cat > "${SRC_DIR}/125_Out_of_bounds_Read.c" << 'EOF'
/* CWE 125: Out-of-bounds Read */
static char buf[16] = {
    1,2,3,4,5,6,7,8,
    9,10,11,12,13,14,15,16
};
volatile int sink;

void start(void) {
    int i;
    int sum = 0;

    for (i = 0; i < 32; i++) {
        sum += buf[i]; /* reads past buf[15] */
    }

    sink = sum;
}
EOF

########################################
# 126 – Buffer Over read
########################################
cat > "${SRC_DIR}/126_Buffer_Over_read.c" << 'EOF'
/* CWE 126: Buffer Over-read */
static char src[8] = {1,2,3,4,5,6,7,8};
static char dst[8];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 16; i++) {
        dst[i] = src[i]; /* over-read src */
    }
    sink = dst[0];
}
EOF

########################################
# 127 – Buffer Under read
########################################
cat > "${SRC_DIR}/127_Buffer_Under_read.c" << 'EOF'
/* CWE 127: Buffer Under-read */
static char buf[16];
volatile int sink;

void start(void) {
    char *p = buf + 8;
    int i;
    int sum = 0;

    for (i = 0; i < 16; i++) {
        sum += p[-i]; /* reads before buf */
    }

    sink = sum;
}
EOF

########################################
# 129 – Improper Validation of Array Index
########################################
cat > "${SRC_DIR}/129_Improper_Validation_of_Array_Index.c" << 'EOF'
/* CWE 129: Improper Validation of Array Index */
static int table[8];
volatile int sink;

void start(void) {
    int i;
    int idx = 16; /* unchecked index */
    for (i = 0; i < 8; i++) {
        table[i] = i;
    }

    sink = table[idx]; /* out-of-bounds read */
}
EOF

########################################
# 131 – Incorrect Calculation of Buffer Size
########################################
cat > "${SRC_DIR}/131_Incorrect_Calculation_of_Buffer_Size.c" << 'EOF'
/* CWE 131: Incorrect Calculation of Buffer Size */
static char buf[16];
volatile int sink;

void start(void) {
    int logical_len = 16;
    int wrong_size = logical_len + 8; /* miscalculation */

    int i;
    for (i = 0; i < wrong_size; i++) {
        buf[i] = (char)i; /* overwrite past end */
    }

    sink = buf[0];
}
EOF

########################################
# 134 – Use of Externally Controlled Format String (simulated)
########################################
cat > "${SRC_DIR}/134_Use_of_Externally_Controlled_Format_String.c" << 'EOF'
/* CWE 134: Use of Externally-Controlled Format String (simulated) */
static char fmt[8] = {'%','x','%','x','%','x','%','x'};
static char buf[8];
volatile int sink;

void start(void) {
    int i;
    /* naive copy using untrusted "format" as length */
    for (i = 0; i < 32; i++) {
        buf[i] = fmt[i]; /* overflow buf */
    }
    sink = buf[0];
}
EOF

########################################
# 188 – Reliance on Data/Memory Layout
########################################
cat > "${SRC_DIR}/188_Reliance_on_Data_Memory_Layout.c" << 'EOF'
/* CWE 188: Reliance on Data/Memory Layout */
struct Pair {
    int a;
    int b;
};

static struct Pair p = {1, 2};
volatile int sink;

void start(void) {
    int *raw = (int *)&p;
    /* assume a,b are contiguous and blindly write two ints */
    raw[0] = 10;
    raw[1] = 20;
    raw[2] = 30; /* overwrite beyond struct */
    sink = p.a + p.b;
}
EOF

########################################
# 198 – Use of Incorrect Byte Ordering
########################################
cat > "${SRC_DIR}/198_Use_of_Incorrect_Byte_Ordering.c" << 'EOF'
/* CWE 198: Use of Incorrect Byte Ordering (simulated) */
static unsigned char bytes[4] = {0x11, 0x22, 0x33, 0x44};
volatile unsigned int sink;

void start(void) {
    /* wrong endianness interpretation, then out-of-bounds tweak */
    unsigned int val =
        ((unsigned int)bytes[3] << 24) |
        ((unsigned int)bytes[2] << 16) |
        ((unsigned int)bytes[1] << 8)  |
        ((unsigned int)bytes[0]);

    bytes[4] = 0xFF; /* out-of-bounds write to emphasize bug */
    sink = val;
}
EOF

########################################
# 244 – Improper Clearing of Heap Memory Before Release
########################################
cat > "${SRC_DIR}/244_Improper_Clearing_of_Heap_Memory_Before_Release_Heap_Inspection.c" << 'EOF'
/* CWE 244: Improper Clearing of Heap Memory Before Release (simulated) */
static char fake_heap[32];
static int allocated = 0;
volatile int sink;

char *my_alloc(int n) {
    if (allocated || n > 32) return 0;
    allocated = 1;
    return fake_heap;
}

void my_free(char *p) {
    (void)p;
    /* memory contains old data, not cleared */
}

void start(void) {
    int i;
    char *p = my_alloc(16);
    if (!p) return;
    for (i = 0; i < 16; i++) {
        p[i] = (char)(i + 1);
    }
    my_free(p);
    /* later, read stale secret */
    sink = fake_heap[0];
}
EOF

########################################
# 401 – Missing Release of Memory after Effective Lifetime
########################################
cat > "${SRC_DIR}/401_Missing_Release_of_Memory_after_Effective_Lifetime.c" << 'EOF'
/* CWE 401: Missing Release of Memory after Effective Lifetime (simulated leak) */
static char fake_heap[64];
static int offset = 0;
volatile int sink;

char *my_alloc(int n) {
    if (offset + n > 64) return 0;
    char *p = fake_heap + offset;
    offset += n;
    return p;
}

void start(void) {
    int i;
    char *p = my_alloc(32);
    if (!p) return;
    for (i = 0; i < 32; i++) {
        p[i] = (char)i;
    }
    /* never freed, leak simulated */
    sink = p[0];
}
EOF

########################################
# 415 – Double Free (simulated)
########################################
cat > "${SRC_DIR}/415_Double_Free.c" << 'EOF'
/* CWE 415: Double Free (simulated) */
static char fake_heap[32];
static int freed = 0;
volatile int sink;

char *my_alloc(void) {
    if (freed) return 0;
    return fake_heap;
}

void my_free(char *p) {
    (void)p;
    if (freed) {
        /* double free state */
        fake_heap[0] = 42; /* scribble */
    }
    freed = 1;
}

void start(void) {
    char *p = my_alloc();
    if (!p) return;
    my_free(p);
    my_free(p); /* double free */
    sink = fake_heap[0];
}
EOF

########################################
# 416 – Use After Free (simulated)
########################################
cat > "${SRC_DIR}/416_Use_After_Free.c" << 'EOF'
/* CWE 416: Use After Free (simulated) */
static char fake_heap[32];
static int in_use = 0;
volatile int sink;

char *my_alloc(void) {
    if (in_use) return 0;
    in_use = 1;
    return fake_heap;
}

void my_free(char *p) {
    (void)p;
    in_use = 0;
}

void start(void) {
    int i;
    char *p = my_alloc();
    if (!p) return;
    for (i = 0; i < 16; i++) p[i] = (char)i;
    my_free(p);
    /* use after free */
    p[0] = 99;
    sink = p[0];
}
EOF

########################################
# 466 – Return of Pointer Value Outside of Expected Range
########################################
cat > "${SRC_DIR}/466_Return_of_Pointer_Value_Outside_of_Expected_Range.c" << 'EOF'
/* CWE 466: Return of Pointer Value Outside of Expected Range (simulated) */
static char buf[16];
volatile int sink;

char *get_ptr_outside(void) {
    return buf + 32; /* outside expected region */
}

void start(void) {
    char *p = get_ptr_outside();
    p[0] = 7; /* invalid location */
    sink = buf[0];
}
EOF

########################################
# 562 – Return of Stack Variable Address
########################################
cat > "${SRC_DIR}/562_Return_of_Stack_Variable_Address.c" << 'EOF'
/* CWE 562: Return of Stack Variable Address */
volatile int sink;

char *bad_func(void) {
    static int toggle = 0;
    char local[8];
    local[0] = (char)(++toggle);
    return local; /* returning pointer to stack */
}

void start(void) {
    char *p = bad_func();
    p[0] = 10; /* writing to invalid stack memory */
    sink = p[0];
}
EOF

########################################
# 587 – Assignment of a Fixed Address to a Pointer
########################################
cat > "${SRC_DIR}/587_Assignment_of_a_Fixed_Address_to_a_Pointer.c" << 'EOF'
/* CWE 587: Assignment of a Fixed Address to a Pointer */
volatile int sink;

void start(void) {
    /* bogus hard-coded address */
    int *p = (int *)0x100;
    *p = 42; /* very likely invalid */
    sink = 1;
}
EOF

########################################
# 590 – Free of Memory not on the Heap (simulated)
########################################
cat > "${SRC_DIR}/590_Free_of_Memory_not_on_the_Heap.c" << 'EOF'
/* CWE 590: Free of Memory not on the Heap (simulated) */
static char fake_heap[32];
volatile int sink;

void my_free(char *p) {
    /* if p not in fake_heap, scribble to show error */
    if (p < fake_heap || p >= fake_heap + 32) {
        fake_heap[0] = 99;
    }
}

void start(void) {
    char local[16];
    my_free(local); /* freeing non-heap */
    sink = fake_heap[0];
}
EOF

########################################
# 680 – Integer Overflow to Buffer Overflow
########################################
cat > "${SRC_DIR}/680_Integer_Overflow_to_Buffer_Overflow.c" << 'EOF'
/* CWE 680: Integer Overflow to Buffer Overflow (simulated) */
static char buf[32];
volatile int sink;

void start(void) {
    unsigned int count = 0xFFFFFFF0u;
    unsigned int elem_size = 4u;
    unsigned int total = count * elem_size; /* overflow in real scenario */

    unsigned int i;
    char *p = buf;
    for (i = 0; i < 64; i++) {
        p[i] = (char)i; /* overflow buf */
    }
    sink = p[0];
}
EOF

########################################
# 690 – Unchecked Return Value to NULL Pointer Dereference
########################################
cat > "${SRC_DIR}/690_Unchecked_Return_Value_to_NULL_Pointer_Dereference.c" << 'EOF'
/* CWE 690: Unchecked Return Value to NULL Pointer Dereference (simulated) */
static char fake_heap[16];
static int allocated = 0;
volatile int sink;

char *my_alloc(int n) {
    if (allocated || n > 16) return 0;
    allocated = 1;
    return fake_heap;
}

void start(void) {
    char *p = my_alloc(32); /* will return 0 */
    p[0] = 1; /* dereference NULL */
    sink = p[0];
}
EOF

########################################
# 761 – Free of Pointer not at Start of Buffer
########################################
cat > "${SRC_DIR}/761_Free_of_Pointer_not_at_Start_of_Buffer.c" << 'EOF'
/* CWE 761: Free of Pointer not at Start of Buffer (simulated) */
static char fake_heap[32];
volatile int sink;

void my_free(char *p) {
    /* expect exact base */
    if (p != fake_heap) {
        fake_heap[0] = 77; /* signal wrong free */
    }
}

void start(void) {
    char *base = fake_heap;
    char *mid = base + 8;
    my_free(mid); /* incorrect pointer passed to free */
    sink = fake_heap[0];
}
EOF

########################################
# 762 – Mismatched Memory Management Routines (simulated)
########################################
cat > "${SRC_DIR}/762_Mismatched_Memory_Management_Routines.c" << 'EOF'
/* CWE 762: Mismatched Memory Management Routines (simulated) */
static char fake_heap[32];
static char other_heap[32];
volatile int sink;

char *my_alloc(void) {
    return fake_heap;
}

void my_free(char *p) {
    if (p == other_heap) {
        other_heap[0] = 11;
    } else if (p == fake_heap) {
        fake_heap[0] = 22;
    }
}

void start(void) {
    char *p = my_alloc();
    my_free(other_heap); /* mismatch: freeing with wrong heap region */
    sink = fake_heap[0] + other_heap[0];
}
EOF

########################################
# 763 – Release of Invalid Pointer or Reference
########################################
cat > "${SRC_DIR}/763_Release_of_Invalid_Pointer_or_Reference.c" << 'EOF'
/* CWE 763: Release of Invalid Pointer or Reference (simulated) */
static char fake_heap[32];
volatile int sink;

void my_free(char *p) {
    if (p < fake_heap || p >= fake_heap + 32) {
        fake_heap[0] = 55; /* invalid pointer */
    }
}

void start(void) {
    char *p = fake_heap + 64; /* definitely outside */
    my_free(p);
    sink = fake_heap[0];
}
EOF

########################################
# 786 – Access of Memory Location Before Start of Buffer
########################################
cat > "${SRC_DIR}/786_Access_of_Memory_Location_Before_Start_of_Buffer.c" << 'EOF'
/* CWE 786: Access of Memory Location Before Start of Buffer */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    char *p = buf;
    for (i = 1; i <= 8; i++) {
        p[-i] = (char)i; /* before buf */
    }
    sink = buf[0];
}
EOF

########################################
# 787 – Out of bounds Write
########################################
cat > "${SRC_DIR}/787_Out_of_bounds_Write.c" << 'EOF'
/* CWE 787: Out-of-bounds Write */
static char buf[16];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 32; i++) {
        buf[i] = (char)i; /* write beyond 16 bytes */
    }
    sink = buf[0];
}
EOF

########################################
# 788 – Access of Memory Location After End of Buffer
########################################
cat > "${SRC_DIR}/788_Access_of_Memory_Location_After_End_of_Buffer.c" << 'EOF'
/* CWE 788: Access of Memory Location After End of Buffer */
static char buf[16];
volatile int sink;

void start(void) {
    char *p = buf;
    int i;
    for (i = 0; i < 16; i++) {
        p[i] = (char)i;
    }
    sink = p[32]; /* read far beyond end */
}
EOF

########################################
# 789 – Memory Allocation with Excessive Size Value (simulated)
########################################
cat > "${SRC_DIR}/789_Memory_Allocation_with_Excessive_Size_Value.c" << 'EOF'
/* CWE 789: Memory Allocation with Excessive Size Value (simulated) */
static char fake_heap[64];
volatile int sink;

char *my_alloc(unsigned int n) {
    if (n > 64u) {
        return 0; /* pretend failure */
    }
    return fake_heap;
}

void start(void) {
    unsigned int n = 1000u;
    char *p = my_alloc(n); /* returns 0 */
    if (!p) {
        p = (char *)0;
    }
    p[0] = 1; /* NULL deref */
    sink = p[0];
}
EOF

########################################
# 805 – Buffer Access with Incorrect Length Value
########################################
cat > "${SRC_DIR}/805_Buffer_Access_with_Incorrect_Length_Value.c" << 'EOF'
/* CWE 805: Buffer Access with Incorrect Length Value */
static char buf[16];
volatile int sink;

void start(void) {
    int length = 32; /* incorrect length */
    int i;
    for (i = 0; i < length; i++) {
        buf[i] = (char)i; /* out-of-bounds write */
    }
    sink = buf[0];
}
EOF

########################################
# 806 – Buffer Access Using Size of Source Buffer
########################################
cat > "${SRC_DIR}/806_Buffer_Access_Using_Size_of_Source_Buffer.c" << 'EOF'
/* CWE 806: Buffer Access Using Size of Source Buffer */
static char src[32];
static char dst[16];
volatile int sink;

void start(void) {
    int i;
    for (i = 0; i < 32; i++) {
        src[i] = (char)i;
    }
    /* use source length for destination write */
    for (i = 0; i < 32; i++) {
        dst[i] = src[i]; /* overflow dst */
    }
    sink = dst[0];
}
EOF

########################################
# 822 – Untrusted Pointer Dereference
########################################
cat > "${SRC_DIR}/822_Untrusted_Pointer_Dereference.c" << 'EOF'
/* CWE 822: Untrusted Pointer Dereference (simulated) */
static int safe = 1;
volatile int sink;

int *get_untrusted_ptr(void) {
    /* in a real system this could come from attacker controlled data */
    return (int *)0; /* clearly invalid here */
}

void start(void) {
    int *p = get_untrusted_ptr();
    *p = 5; /* dereference untrusted pointer */
    sink = safe;
}
EOF

########################################
# 823 – Use of Out of range Pointer Offset
########################################
cat > "${SRC_DIR}/823_Use_of_Out_of_range_Pointer_Offset.c" << 'EOF'
/* CWE 823: Use of Out-of-range Pointer Offset */
static char buf[16];
volatile int sink;

void start(void) {
    char *p = buf + 4;
    char *q = p + 32; /* out-of-range offset */
    q[0] = 9;
    sink = buf[0];
}
EOF

########################################
# 824 – Access of Uninitialized Pointer
########################################
cat > "${SRC_DIR}/824_Access_of_Uninitialized_Pointer.c" << 'EOF'
/* CWE 824: Access of Uninitialized Pointer */
volatile int sink;

void start(void) {
    char *p;
    /* p is never initialized */
    p[0] = 3;
    sink = p[0];
}
EOF

########################################
# 825 – Expired Pointer Dereference
########################################
cat > "${SRC_DIR}/825_Expired_Pointer_Dereference.c" << 'EOF'
/* CWE 825: Expired Pointer Dereference (simulated) */
static char fake_heap[32];
static int in_use = 0;
volatile int sink;

char *my_alloc(void) {
    if (in_use) return 0;
    in_use = 1;
    return fake_heap;
}

void my_free(char *p) {
    (void)p;
    in_use = 0;
}

void start(void) {
    int i;
    char *p = my_alloc();
    if (!p) return;
    for (i = 0; i < 16; i++) {
        p[i] = (char)i;
    }
    my_free(p);
    /* pointer p has expired but is reused */
    p[0] = 77;
    sink = p[0];
}
EOF

echo "Done. Generated CWE benchmark C files under: ${SRC_DIR}"
