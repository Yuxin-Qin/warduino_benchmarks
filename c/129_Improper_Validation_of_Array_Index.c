/* CWE 129: Improper Validation of Array Index */

volatile int sink;

void start(void) {
    int arr[8];
    int idx = 16; /* unvalidated external value (simulated) */

    arr[0] = 1;

    /* use index without checking range */
    arr[idx] = 42; /* out-of-bounds write */

    sink = arr[0];
}
