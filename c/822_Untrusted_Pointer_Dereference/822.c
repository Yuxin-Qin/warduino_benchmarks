/* CWE-822 */
volatile int sink;
volatile unsigned long untrusted_addr = 0x2000u;
void start(void) {
    int *p = (int *)untrusted_addr;
    *p = 42;  /* dereference untrusted pointer */
    sink = *p;
}

