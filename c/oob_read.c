// Intentional memory weakness: out-of-bounds read
void start() {
    volatile int arr[4] = {1, 2, 3, 4};
    volatile int sum = 0;
    // Read past the end of arr
    for (int i = 0; i < 8; i++) {
        sum += arr[i];
    }
    (void)sum;
}
