#!/usr/bin/env bash
set -euo pipefail

# Create a folder for CWE-823 benchmarks and populate it with 10 C programs.
# Each program demonstrates "Use of Out-of-range Pointer Offset" in a slightly
# different way, using only raw C (no standard library) and a 'start' entry.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_DIR="${SCRIPT_DIR}/../c/823_Use_of_Out_of_range_Pointer_Offset"

mkdir -p "${OUT_DIR}"

############################################################
# 823_1.c  – simple out-of-range pointer within big buffer
############################################################
cat > "${OUT_DIR}/823_1.c" <<'EOF'
volatile int sink;

static int big_buf[64];

void start(void) {
    /* Region: logical elements [16..31] inside big_buf */
    int *region = &big_buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        region[i] = i;
    }

    /* Out-of-range pointer offset: step 20 ints forward from region base.
       This lands inside big_buf but outside the logical region. */
    int *p = region + 20;
    *p = 999;

    sink = region[0];
}
EOF

############################################################
# 823_2.c  – stepping past the end of a subarray "window"
############################################################
cat > "${OUT_DIR}/823_2.c" <<'EOF'
volatile int sink;

static int buffer[128];

void start(void) {
    /* Define a logical window of length 8 inside buffer */
    int *window = &buffer[32];
    int logical_len = 8;
    int i;

    for (i = 0; i < logical_len; i++) {
        window[i] = i + 10;
    }

    /* Incorrect offset: write at index 12 instead of < 8.
       Still within 'buffer', but outside logical window. */
    int *p = window + 12;
    *p = 777;

    sink = window[0];
}
EOF

############################################################
# 823_3.c  – crossing region boundary inside a single array
############################################################
cat > "${OUT_DIR}/823_3.c" <<'EOF'
volatile int sink;

static int arena[64];

void start(void) {
    /* Two logical regions inside one arena */
    int *region_a = &arena[0];   /* [0..15]   */
    int *region_b = &arena[16];  /* [16..31]  */
    int i;

    for (i = 0; i < 16; i++) {
        region_a[i] = i;
        region_b[i] = 100 + i;
    }

    /* Bug: code thinks region_a has length 20 and writes into region_b. */
    int *p = region_a + 18;
    *p = 0x1111;

    sink = region_b[0];
}
EOF

############################################################
# 823_4.c  – underflow within an arena (before region)
############################################################
cat > "${OUT_DIR}/823_4.c" <<'EOF'
volatile int sink;

static int pool[64];

void start(void) {
    /* Logical region starts at pool[24], length 8: [24..31] */
    int *region = &pool[24];
    int i;

    for (i = 0; i < 8; i++) {
        region[i] = 200 + i;
    }

    /* Out-of-range pointer offset: step backwards beyond region start.
       This still stays inside 'pool' but violates region bounds. */
    int *p = region - 4;
    *p = 0x2222;

    sink = region[0];
}
EOF

############################################################
# 823_5.c  – struct field used as a smaller region
############################################################
cat > "${OUT_DIR}/823_5.c" <<'EOF'
volatile int sink;

struct Block {
    int header[4];   /* meta-data */
    int payload[8];  /* logical region */
    int trailer[4];  /* padding */
};

static struct Block blk;

void start(void) {
    int *payload = blk.payload;
    int i;

    for (i = 0; i < 8; i++) {
        payload[i] = 300 + i;
    }

    /* Bug: code thinks payload has length 12 and writes into trailer. */
    int *p = payload + 10;  /* crosses into blk.trailer */
    *p = 0x3333;

    sink = blk.payload[0];
}
EOF

############################################################
# 823_6.c  – overlapping “logical regions” in a single array
############################################################
cat > "${OUT_DIR}/823_6.c" <<'EOF'
volatile int sink;

static int arena[96];

void start(void) {
    int *region1 = &arena[8];   /* [8..23]   */
    int *region2 = &arena[24];  /* [24..39]  */
    int i;

    for (i = 0; i < 16; i++) {
        region1[i] = 400 + i;
        region2[i] = 500 + i;
    }

    /* Code assumes region1 is length 24 and marches into region2. */
    int *p = region1 + 20;  /* inside arena, outside intended region1 */
    *p = 0x4444;

    sink = region2[0];
}
EOF

############################################################
# 823_7.c  – misuse of an index range on a subregion
############################################################
cat > "${OUT_DIR}/823_7.c" <<'EOF'
volatile int sink;

static int storage[128];

void start(void) {
    /* Region is supposed to be indices [40..55], length 16 */
    int *region = &storage[40];
    int logical_len = 16;
    int i;

    for (i = 0; i < logical_len; i++) {
        region[i] = 600 + i;
    }

    /* Off-by-chunk bug: multiplies index by 2 instead of 1. */
    int bad_index = 10 * 2;  /* 20, valid index would be < 16 */
    int *p = region + bad_index;
    *p = 0x5555;

    sink = region[0];
}
EOF

############################################################
# 823_8.c  – “sub-region” carved from a large global arena
############################################################
cat > "${OUT_DIR}/823_8.c" <<'EOF'
volatile int sink;

static int global_arena[256];

void start(void) {
    /* Sub-region advertised to caller as length 32 starting at 64. */
    int *sub = &global_arena[64];
    int advertised_len = 32;
    int i;

    for (i = 0; i < advertised_len; i++) {
        sub[i] = 700 + i;
    }

    /* Caller miscalculates length and writes beyond end of sub-region. */
    int wrong_len = advertised_len + 16;
    int *p = sub + wrong_len;  /* still inside global_arena */
    *p = 0x6666;

    sink = sub[0];
}
EOF

############################################################
# 823_9.c  – region within an arena “header + payload”
############################################################
cat > "${OUT_DIR}/823_9.c" <<'EOF'
volatile int sink;

static int arena[80];

void start(void) {
    int *header  = &arena[0];   /* [0..7]   */
    int *payload = &arena[8];   /* [8..39]  */
    int i;

    for (i = 0; i < 8; i++) {
        header[i] = 10 + i;
    }
    for (i = 0; i < 32; i++) {
        payload[i] = 800 + i;
    }

    /* Bug: treats payload as 40 elements long, overwriting after it. */
    int *p = payload + 36;
    *p = 0x7777;

    sink = header[0];
}
EOF

############################################################
# 823_10.c – sub-region before another, with off-by-one stride
############################################################
cat > "${OUT_DIR}/823_10.c" <<'EOF'
volatile int sink;

static int area[96];

void start(void) {
    int *front = &area[16];  /* [16..31] */
    int *back  = &area[32];  /* [32..47] */
    int i;

    for (i = 0; i < 16; i++) {
        front[i] = 900 + i;
        back[i]  = 1000 + i;
    }

    /* Incorrect stepping: skips by 3 instead of 1 in the front region. */
    int idx = 0;
    while (idx < 8) {
        front[idx] = 0x1234;
        idx += 3;  /* jumps beyond 16 after a few iterations */
    }

    /* Final write beyond intended region front, still in area. */
    int *p = front + 20;
    *p = 0x8888;

    sink = back[0];
}
EOF

echo "Created 10 CWE-823 benchmarks in: ${OUT_DIR}"
