/* CWE 562: Return of Stack Variable Address */
volatile int sink;

char *bad_func(void) {
    static int toggle = 0;
    char local[8];
    local[0] = (char)(++toggle);
    return local; /* returning pointer to stack */
}

void start(void) {
    char *p = bad_func();
    p[0] = 10; /* writing to invalid stack memory */
    sink = p[0];
}
