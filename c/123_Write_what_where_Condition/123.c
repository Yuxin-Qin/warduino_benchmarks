/* CWE-123 */
volatile int victim = 0;
volatile int *target_ptr = &victim;
volatile int attacker_value = 0x5555;

void start(void) {
    volatile int **pp = (volatile int **)&target_ptr;
    /* attacker corrupts pointer */
    *pp = (volatile int *)((unsigned long)pp + 16);
    **pp = attacker_value;   /* uncontrolled write-what-where */
}

