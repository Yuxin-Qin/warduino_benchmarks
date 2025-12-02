/*
 * Return a pointer to a stack-allocated object and use it later.
 */

static int *global_ptr;

static void leak_stack_pointer(void) {
    int local = 7;
    global_ptr = &local; /* escape stack pointer */
}

volatile int sink;

int main(void) {
    leak_stack_pointer();
    /* Use pointer to dead stack frame. */
    sink = *global_ptr;
    return 0;
}
