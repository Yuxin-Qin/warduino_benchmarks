#!/usr/bin/env bash
set -euo pipefail

# Root of the repo (warduino_benchmarks)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"

# New directory for wasm-specific C sources
WASM_SRC_DIR="$ROOT_DIR/c_wasm"

mkdir -p "$WASM_SRC_DIR"

echo "Generating Wasm-friendly C benchmarks in: $WASM_SRC_DIR"
echo

#############################
# 1) buffer_overflow.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/buffer_overflow.c"
/*
 * Intentional stack buffer overflow:
 * write 16 bytes into an 8-byte array.
 * No headers, no libc.
 */
int main(void) {
    char buf[8];
    int i;
    for (i = 0; i < 16; ++i) {
        buf[i] = (char)i; /* out-of-bounds writes */
    }
    return (int)buf[0];
}
EOF

#############################
# 2) oob_read.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/oob_read.c"
/*
 * Out-of-bounds read from a small array.
 */
volatile int sink;

int main(void) {
    int arr[4] = {1, 2, 3, 4};
    int sum = 0;
    int i;
    for (i = 0; i < 8; ++i) { /* read past end */
        sum += arr[i];
    }
    sink = sum;
    return 0;
}
EOF

#############################
# 3) bad_pointer_arith.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/bad_pointer_arith.c"
/*
 * Invalid pointer arithmetic producing a pointer
 * far away from the original object.
 */
volatile int sink;

int main(void) {
    int x = 42;
    char *base = (char *)&x;
    /* Intentionally walk far beyond object bounds. */
    int *p = (int *)(base + 1024);
    sink = *p; /* invalid dereference */
    return 0;
}
EOF

#############################
# 4) bad_struct_layout.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/bad_struct_layout.c"
/*
 * Misusing struct layout by reinterpreting memory
 * as a different struct type.
 */
struct A {
    int a;
    int b;
};

struct B {
    int x;
    int y;
    int z;
};

volatile int sink;

int main(void) {
    struct A obj = {1, 2};
    struct B *p = (struct B *)&obj; /* incorrect layout assumption */
    sink = p->z; /* access field that does not exist */
    return 0;
}
EOF

#############################
# 5) misaligned_access.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/misaligned_access.c"
/*
 * Force a misaligned int* access by offsetting a char buffer.
 */
volatile int sink;

int main(void) {
    char buf[8];
    /* Intentionally misaligned pointer (implementation-dependent). */
    int *p = (int *)(buf + 1);
    *p = 123; /* potentially misaligned write */
    sink = *p;
    return 0;
}
EOF

#############################
# 6) uninitialized.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/uninitialized.c"
/*
 * Use an uninitialized local variable.
 */
volatile int sink;

int main(void) {
    int x; /* uninitialized */
    sink = x; /* use without initialization */
    return 0;
}
EOF

#############################
# 7) escape_stack.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/escape_stack.c"
/*
 * Return a pointer to a stack-allocated object and use it later.
 */

static int *global_ptr;

static void leak_stack_pointer(void) {
    int local = 7;
    global_ptr = &local; /* escape stack pointer */
}

volatile int sink;

int main(void) {
    leak_stack_pointer();
    /* Use pointer to dead stack frame. */
    sink = *global_ptr;
    return 0;
}
EOF

#############################
# 8) stale_pointer.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/stale_pointer.c"
/*
 * Stale pointer pattern using a simple bump allocator.
 * No real free, but simulates reusing a region.
 */

static char heap_area[32];
static int heap_offset = 0;

static void *my_alloc(int size) {
    if (heap_offset + size > (int)sizeof(heap_area)) {
        return 0;
    }
    void *p = &heap_area[heap_offset];
    heap_offset += size;
    return p;
}

/* No-op free; we just simulate conceptual lifetime issues. */
static void my_free(void *p) {
    (void)p;
}

volatile char sink;

int main(void) {
    char *p1 = (char *)my_alloc(8);
    char *p2;

    p1[0] = 1;
    my_free(p1); /* conceptually freed */

    /* Allocate again from the same pool */
    p2 = (char *)my_alloc(8);
    p2[0] = 2;

    /* Stale pointer p1 still used */
    sink = p1[0];
    return 0;
}
EOF

#############################
# 9) use_after_free.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/use_after_free.c"
/*
 * Use-after-free-like pattern with a local allocator.
 * No libc malloc/free.
 */

static char heap_area[16];
static int heap_used = 0;

static void *my_alloc(int size) {
    if (heap_used + size > (int)sizeof(heap_area)) {
        return 0;
    }
    void *p = &heap_area[heap_used];
    heap_used += size;
    return p;
}

/* Simulated free: does not actually reclaim memory. */
static void my_free(void *p) {
    (void)p;
}

volatile char sink;

int main(void) {
    char *p = (char *)my_alloc(8);
    if (!p) {
        return 0;
    }
    p[0] = 42;
    my_free(p);
    /* Use pointer after conceptual free. */
    p[1] = 13;
    sink = p[1];
    return 0;
}
EOF

#############################
# 10) double_free.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/double_free.c"
/*
 * Double-free-like pattern with a simple allocator.
 */

static char heap_area[16];
static int heap_used = 0;

static void *my_alloc(int size) {
    if (heap_used + size > (int)sizeof(heap_area)) {
        return 0;
    }
    void *p = &heap_area[heap_used];
    heap_used += size;
    return p;
}

static int freed_flag = 0;

static void my_free(void *p) {
    (void)p;
    if (freed_flag) {
        /* second free on same conceptual object */
        /* in a real allocator this would be a bug */
    }
    freed_flag = 1;
}

int main(void) {
    char *p = (char *)my_alloc(8);
    if (!p) {
        return 0;
    }
    my_free(p);
    my_free(p); /* double free pattern */
    return 0;
}
EOF

#############################
# 11) funcptr_overwrite.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/funcptr_overwrite.c"
/*
 * Overwrite a function pointer via buffer overflow.
 */

typedef void (*func_t)(void);

static int flag = 0;

static void safe_function(void) {
    flag = 1;
}

static void evil_function(void) {
    flag = 2;
}

int main(void) {
    char buf[16];
    func_t fp = safe_function;

    /* Layout: buf[] followed by fp on the stack (approximate).
       The exact placement is implementation-dependent, but this
       pattern captures the idea of smashing the function pointer. */
    int i;
    for (i = 0; i < 32; ++i) {
        buf[i] = (char)((unsigned long)evil_function >> (i % 8));
    }

    fp(); /* potentially corrupted */
    return flag;
}
EOF

#############################
# 12) retaddr_overwrite.c
#############################
cat <<'EOF' > "$WASM_SRC_DIR/retaddr_overwrite.c"
/*
 * Stack smashing pattern that could overwrite a saved return address
 * in a traditional ABI. Here we just model a large overflow.
 */

static int marker = 0;

static void vulnerable(void) {
    char buf[16];
    int i;
    for (i = 0; i < 64; ++i) {
        buf[i] = (char)i; /* writes beyond buf[] */
    }
    marker = 1;
}

int main(void) {
    vulnerable();
    return marker;
}
EOF

#############################
# 13) data_race.c (sequential pattern only)
#############################
cat <<'EOF' > "$WASM_SRC_DIR/data_race.c"
/*
 * Placeholder for a data race pattern.
 * Real data races require concurrency primitives which are not
 * available here (no threads, no libc). We simulate two writers
 * writing without synchronization, but sequentially.
 */

volatile int shared_var = 0;

static void writer1(void) {
    shared_var = 1;
}

static void writer2(void) {
    shared_var = 2;
}

int main(void) {
    writer1();
    writer2();
    return shared_var;
}
EOF

echo "Done. Generated wasm-specific benchmarks in $WASM_SRC_DIR"
