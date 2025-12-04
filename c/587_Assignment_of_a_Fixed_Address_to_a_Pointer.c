/* CWE 587: Assignment of a Fixed Address to a Pointer */
volatile int sink;

void start(void) {
    /* bogus hard-coded address */
    int *p = (int *)0x100;
    *p = 42; /* very likely invalid */
    sink = 1;
}
