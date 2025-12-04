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
