/* CWE 762: Mismatched Memory Management Routines (simulated) */

volatile int sink;
static char heap_area[32];

void my_free(char *p) {
    sink += (int)(unsigned long)p;
}

void start(void) {
    char *p = heap_area; /* pretend this came from a different allocator */
    my_free(p); /* mismatched "free" */
}
