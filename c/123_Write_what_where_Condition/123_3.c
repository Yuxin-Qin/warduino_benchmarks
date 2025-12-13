/* CWE-123 variant 3 */
volatile int victim = 0;
volatile int *target_ptr = &victim;
volatile int attacker_value = 0x1000 + 3;

void start(void) {
    volatile int **pp = (volatile int **)&target_ptr;
    /* attacker controls both what and where */
    *pp = (volatile int *)((unsigned long)pp + 3);
    **pp = attacker_value;
}
