// Intentional memory weakness: stack pointer escaping its scope
static volatile int *global_ptr;

static void leak_stack_address(void) {
    volatile int x = 7;
    global_ptr = (int *)&x;
}

void start() {
    leak_stack_address();
    // Use escaped stack pointer after the function returns
    *global_ptr = 99;
}
