/* CWE-129 */
volatile int sink;
void start(void) {
    int arr[8];
    int idx = 20;  /* unchecked index */
    arr[idx] = 42; /* out-of-bounds write */
    sink = arr[0];
}

