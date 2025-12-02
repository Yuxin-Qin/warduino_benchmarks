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
