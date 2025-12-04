/* CWE 129: Improper Validation of Array Index */
static int table[8];
volatile int sink;

void start(void) {
    int i;
    int idx = 16; /* unchecked index */
    for (i = 0; i < 8; i++) {
        table[i] = i;
    }

    sink = table[idx]; /* out-of-bounds read */
}
