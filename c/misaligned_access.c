// Intentional memory weakness: misaligned access
void start() {
    volatile char buf[16];
    // Force a misaligned int* by offsetting one byte
    volatile int *ptr = (int *)(buf + 1);
    *ptr = 0x12345678;
}
