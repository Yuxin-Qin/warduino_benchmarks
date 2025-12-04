/* CWE 823: Use of Out-of-range Pointer Offset */

volatile int sink;

void start(void) {
    int arr[4];
    int *p = arr;
    int *q = p + 10; /* out-of-range offset */

    *q = 5; /* write beyond array */

    sink = arr[0];
}
