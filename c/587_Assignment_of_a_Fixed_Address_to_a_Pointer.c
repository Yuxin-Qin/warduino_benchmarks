/* CWE 587: Assignment of a Fixed Address to a Pointer */

volatile int sink;

void start(void) {
    /* arbitrary fixed address, invalid in normal execution */
    int *p = (int *)0x100u;

    /* dereference invalid pointer */
    *p = 42;

    sink = *p;
}
