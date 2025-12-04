/* CWE 824: Access of Uninitialized Pointer */
volatile int sink;

void start(void) {
    char *p;
    /* p is never initialized */
    p[0] = 3;
    sink = p[0];
}
