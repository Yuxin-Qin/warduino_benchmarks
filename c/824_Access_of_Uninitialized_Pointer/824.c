/* CWE-824 */
volatile int sink;
void start(void) {
    int *p;
    *p = 42;  /* use of uninitialized pointer */
    sink = *p;
}

