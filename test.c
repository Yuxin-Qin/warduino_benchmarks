/* file: cheri_vs_noncheri_ownmalloc.c
 *
 * Intentional heap out-of-bounds write via a tiny custom allocator.
 * - No standard library calls (no malloc, no printf, no includes).
 * - On a non-CHERI machine: usually runs without crashing (silent UB).
 * - On CHERI purecap: should trigger a capability violation
 *   when the pointer goes out of the heap capability bounds.
 */

/* A small, fixed-size "heap". */
static char heap[64];
static int heap_used = 0;

/* Something we do NOT want to corrupt. */
volatile int guard = 12345;

/* Very simple bump-pointer allocator:
 * returns 0 if there is not enough space.
 */
void *my_malloc(int n)
{
    char *p;

    if (n <= 0) {
        return (void *)0;
    }

    if (heap_used + n > (int)sizeof(heap)) {
        /* Out of heap space – return NULL-like pointer. */
        return (void *)0;
    }

    p = heap + heap_used;
    heap_used += n;
    return (void *)p;
}

int start(void)
{
    /* Allocate 16 bytes (4 ints if int is 4 bytes). */
    int *p = (int *)my_malloc(16);
    int i;

    if (!p) {
        /* Allocation failed – just return an error code. */
        return 1;
    }

    /* In-bounds initialization. */
    for (i = 0; i < 4; i++) {
        p[i] = i;
    }

    /* BUG: massive out-of-bounds writes.
     * We only reserved 16 bytes in a 64-byte heap,
     * but we now write 100 extra ints.
     *
     * - On non-CHERI: undefined behaviour, but often no crash.
     *   Just silent memory corruption somewhere after 'heap'.
     * - On CHERI: once the effective address leaves the
     *   capability bounds for 'heap', a capability fault
     *   should be raised.
     */
    for (i = 0; i < 100; i++) {
        p[4 + i] = 0xDEAD0000 + i;
    }

    /* See whether the global guard got corrupted. */
    if (guard == 12345) {
        /* Looks "fine" (but memory may be badly corrupted). */
        return 0;
    } else {
        /* Guard changed → clear symptom of the bug. */
        return 2;
    }
}
