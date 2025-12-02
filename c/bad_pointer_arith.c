// Intentional memory weakness: invalid pointer arithmetic
void start() {
    volatile int arr[4] = {10, 20, 30, 40};
    volatile char *p = (char *)arr;
    // Step into the middle of an int and reinterpret as int*
    volatile int *bad = (int *)(p + 1);
    *bad = 123; // Misaligned, out-of-bounds style write
}
