/* CWE 562: Return of Stack Variable Address */

volatile int sink;

char *get_stack_buffer(void) {
    char buf[16];
    int i;
    for (i = 0; i < 16; i++) {
        buf[i] = (char)(i + 1);
    }
    return buf; /* returning stack address */
}

void start(void) {
    char *dangling = get_stack_buffer();
    int i;

    for (i = 0; i < 16; i++) {
        sink += dangling[i]; /* use of invalid stack pointer */
    }
}
