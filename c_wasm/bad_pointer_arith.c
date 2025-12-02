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
