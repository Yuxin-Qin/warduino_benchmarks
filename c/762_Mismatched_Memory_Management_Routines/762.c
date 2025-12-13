/* CWE-762 */
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

