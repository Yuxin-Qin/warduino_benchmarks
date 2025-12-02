// Intentional memory weakness: simulated data race (no threads here)

static volatile int shared = 0;

static void writer1(void) {
    for (int i = 0; i < 1000; i++) {
        shared = shared + 1;
    }
}

static void writer2(void) {
    for (int i = 0; i < 1000; i++) {
        shared = shared - 1;
    }
}

void start() {
    // In a real program these would run concurrently on different threads.
    writer1();
    writer2();
}
