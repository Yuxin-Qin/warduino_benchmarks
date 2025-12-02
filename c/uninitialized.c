// Intentional memory weakness: use of uninitialized stack data
void start() {
    volatile int x;
    // x is never given a defined initial value
    volatile int y = x + 1;
    (void)y;
}
