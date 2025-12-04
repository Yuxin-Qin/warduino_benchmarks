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
