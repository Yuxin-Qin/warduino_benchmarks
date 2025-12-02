#!/usr/bin/env bash
set -euo pipefail

# Root of the warduino_benchmarks repo (script/..)
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
C_DIR="$ROOT/c"

mkdir -p "$C_DIR"

echo "[init_c] Root:  $ROOT"
echo "[init_c] C dir: $C_DIR"
echo "[init_c] Generating minimal no-libc benchmarks with void start() entrypoint"

############################################################
# 1) buffer_overflow.c
############################################################
cat > "$C_DIR/buffer_overflow.c" << 'EOF'
// Intentional memory weakness: stack buffer overflow
void start() {
    volatile char buf[8];
    // Write past the end of buf
    for (int i = 0; i < 16; i++) {
        buf[i] = (char)i;
    }
}
EOF

############################################################
# 2) oob_read.c
############################################################
cat > "$C_DIR/oob_read.c" << 'EOF'
// Intentional memory weakness: out-of-bounds read
void start() {
    volatile int arr[4] = {1, 2, 3, 4};
    volatile int sum = 0;
    // Read past the end of arr
    for (int i = 0; i < 8; i++) {
        sum += arr[i];
    }
    (void)sum;
}
EOF

############################################################
# 3) bad_pointer_arith.c
############################################################
cat > "$C_DIR/bad_pointer_arith.c" << 'EOF'
// Intentional memory weakness: invalid pointer arithmetic
void start() {
    volatile int arr[4] = {10, 20, 30, 40};
    volatile char *p = (char *)arr;
    // Step into the middle of an int and reinterpret as int*
    volatile int *bad = (int *)(p + 1);
    *bad = 123; // Misaligned, out-of-bounds style write
}
EOF

############################################################
# 4) misaligned_access.c
############################################################
cat > "$C_DIR/misaligned_access.c" << 'EOF'
// Intentional memory weakness: misaligned access
void start() {
    volatile char buf[16];
    // Force a misaligned int* by offsetting one byte
    volatile int *ptr = (int *)(buf + 1);
    *ptr = 0x12345678;
}
EOF

############################################################
# 5) uninitialized.c
############################################################
cat > "$C_DIR/uninitialized.c" << 'EOF'
// Intentional memory weakness: use of uninitialized stack data
void start() {
    volatile int x;
    // x is never given a defined initial value
    volatile int y = x + 1;
    (void)y;
}
EOF

############################################################
# 6) stale_pointer.c
############################################################
cat > "$C_DIR/stale_pointer.c" << 'EOF'
// Intentional memory weakness: stale pointer into a local array
static volatile char *saved;

static void make_pointer(void) {
    volatile char local[8];
    for (int i = 0; i < 8; i++) {
        ((char *)local)[i] = (char)i;
    }
    saved = (char *)local; // pointer to stack memory that will go out of scope
}

void start() {
    make_pointer();
    // Use stale pointer after frame is gone
    saved[0] = 42;
}
EOF

############################################################
# 7) escape_stack.c
############################################################
cat > "$C_DIR/escape_stack.c" << 'EOF'
// Intentional memory weakness: stack pointer escaping its scope
static volatile int *global_ptr;

static void leak_stack_address(void) {
    volatile int x = 7;
    global_ptr = (int *)&x;
}

void start() {
    leak_stack_address();
    // Use escaped stack pointer after the function returns
    *global_ptr = 99;
}
EOF

############################################################
# 8) use_after_free.c   (simulated tiny allocator)
############################################################
cat > "$C_DIR/use_after_free.c" << 'EOF'
// Intentional memory weakness: conceptual use-after-free
// We simulate a tiny heap and a dummy "free" to keep everything self-contained.

static char heap[64];
static int used = 0;

static void *my_alloc(int sz) {
    if (used + sz > (int)sizeof(heap)) {
        return 0;
    }
    void *p = &heap[used];
    used += sz;
    return p;
}

static void my_free(void *p) {
    // no real reclamation; just a placeholder
    (void)p;
}

void start() {
    char *p = (char *)my_alloc(16);
    if (!p) return;
    for (int i = 0; i < 16; i++) {
        p[i] = (char)i;
    }
    my_free(p);
    // Conceptual use-after-free: keep writing through the pointer
    p[0] = 42;
    p[15] = 99;
}
EOF

############################################################
# 9) double_free.c  (simulated logical double free)
############################################################
cat > "$C_DIR/double_free.c" << 'EOF'
// Intentional memory weakness: conceptual double free
// Same tiny allocator as use_after_free.c

static char heap[64];
static int used = 0;

static void *my_alloc(int sz) {
    if (used + sz > (int)sizeof(heap)) {
        return 0;
    }
    void *p = &heap[used];
    used += sz;
    return p;
}

static void my_free(void *p) {
    // no real reclamation
    (void)p;
}

void start() {
    char *p = (char *)my_alloc(16);
    if (!p) return;
    p[0] = 1;
    my_free(p);
    // Conceptual double free
    my_free(p);
}
EOF

############################################################
# 10) funcptr_overwrite.c
############################################################
cat > "$C_DIR/funcptr_overwrite.c" << 'EOF'
// Intentional memory weakness: overwrite of function pointer via nearby buffer

typedef int (*fn_ptr)(void);

static int safe_func(void) {
    return 1;
}

static int evil_func(void) {
    // never intended to be called in a safe program
    return 42;
}

void start() {
    volatile char buf[16];
    volatile fn_ptr fp = safe_func;

    // Deliberately scribble into memory near fp.
    // On some layouts this can overwrite fp (undefined behaviour).
    for (int i = 0; i < 64; i++) {
        ((char *)&buf)[i] = (char)i;
    }

    // Call through possibly corrupted function pointer
    int result = fp();
    (void)result;
}
EOF

############################################################
# 11) retaddr_overwrite.c
############################################################
cat > "$C_DIR/retaddr_overwrite.c" << 'EOF'
// Intentional memory weakness: attempt to smash stack near return address
// This pattern is highly implementation-dependent and may not always trigger.

static int victim(void) {
    volatile char buf[32];
    for (int i = 0; i < 256; i++) {
        ((char *)&buf)[i] = (char)i; // writes far past buf
    }
    return 0;
}

void start() {
    int r = victim();
    (void)r;
}
EOF

############################################################
# 12) data_race.c
############################################################
cat > "$C_DIR/data_race.c" << 'EOF'
// Intentional memory weakness: simulated data race (no threads here)

static volatile int shared = 0;

static void writer1(void) {
    for (int i = 0; i < 1000; i++) {
        shared = shared + 1;
    }
}

static void writer2(void) {
    for (int i = 0; i < 1000; i++) {
        shared = shared - 1;
    }
}

void start() {
    // In a real program these would run concurrently on different threads.
    writer1();
    writer2();
}
EOF

############################################################
# 13) bad_struct_layout.c
############################################################
cat > "$C_DIR/bad_struct_layout.c" << 'EOF'
// Intentional memory weakness: aliasing fields via incorrect struct layout

struct A {
    int x;
    int y;
};

struct B {
    int x;
    char small[2];
};

void start() {
    struct A a;
    a.x = 10;
    a.y = 20;

    // Reinterpret A as B and write into small
    struct B *b = (struct B *)&a;
    b->small[0] = 1;
    b->small[1] = 2;
}
EOF

echo "[init_c] Done. Generated $(ls "$C_DIR"/*.c | wc -l | tr -d ' ') C files in $C_DIR"
