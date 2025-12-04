/* CWE 824: Access of Uninitialized Pointer */

volatile int sink;

void start(void) {
    int *p; /* uninitialized pointer */

    *p = 42; /* undefined behavior */

    sink = *p;
}
