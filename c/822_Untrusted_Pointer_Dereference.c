/* CWE 822: Untrusted Pointer Dereference */

volatile int sink;

void start(void) {
    unsigned long attacker_value = 0x8u;
    int *p = (int *)attacker_value; /* untrusted pointer */

    *p = 123; /* arbitrary write via untrusted pointer */

    sink = *p;
}
