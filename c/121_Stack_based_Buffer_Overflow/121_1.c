volatile int sink;

void start(void) {
    int stack_buf[16];
    int i;

    for (i = 0; i < 16; i++) {
        stack_buf[i] = i;
    }

    /* Stack-based overflow: write beyond end of stack_buf. */
    for (i = 16; i < 32; i++) {
        stack_buf[i] = i * 2;   /* out-of-bounds on stack */
    }

    sink = stack_buf[0];
}

