/* CWE 590: Free of Memory not on the Heap (simulated) */

volatile int sink;

void my_free(char *p) {
    /* pretend this only accepts heap pointers; we misuse with stack */
    sink += (int)(unsigned long)(p != 0);
}

void start(void) {
    char buf[16];
    my_free(buf); /* freeing stack memory */
}
