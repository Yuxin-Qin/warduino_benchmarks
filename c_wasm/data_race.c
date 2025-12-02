/*
 * Placeholder for a data race pattern.
 * Real data races require concurrency primitives which are not
 * available here (no threads, no libc). We simulate two writers
 * writing without synchronization, but sequentially.
 */

volatile int shared_var = 0;

static void writer1(void) {
    shared_var = 1;
}

static void writer2(void) {
    shared_var = 2;
}

int main(void) {
    writer1();
    writer2();
    return shared_var;
}
