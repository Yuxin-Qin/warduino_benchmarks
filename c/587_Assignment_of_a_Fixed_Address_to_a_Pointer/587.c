/* CWE-587 */
volatile int sink;
void start(void) {
    int *p = (int *)0x1000u;  /* fixed address */
    *p = 123;
    sink = *p;
}

