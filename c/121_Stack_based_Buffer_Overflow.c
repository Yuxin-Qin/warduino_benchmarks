/* CWE 121: Stack-based Buffer Overflow */
volatile int sink;

void start(void) {
    char local[8];
    int i;

    for (i = 0; i < 24; i++) {
        local[i] = (char)i; /* overflow stack buffer */
    }

    sink = local[0];
}
