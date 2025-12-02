/*
 * Out-of-bounds read from a small array.
 */
volatile int sink;

int main(void) {
    int arr[4] = {1, 2, 3, 4};
    int sum = 0;
    int i;
    for (i = 0; i < 8; ++i) { /* read past end */
        sum += arr[i];
    }
    sink = sum;
    return 0;
}
