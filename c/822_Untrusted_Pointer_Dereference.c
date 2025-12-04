/* CWE 822: Untrusted Pointer Dereference (simulated) */
static int safe = 1;
volatile int sink;

int *get_untrusted_ptr(void) {
    /* in a real system this could come from attacker controlled data */
    return (int *)0; /* clearly invalid here */
}

void start(void) {
    int *p = get_untrusted_ptr();
    *p = 5; /* dereference untrusted pointer */
    sink = safe;
}
