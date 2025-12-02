// Intentional memory weakness: attempt to smash stack near return address
// This pattern is highly implementation-dependent and may not always trigger.

static int victim(void) {
    volatile char buf[32];
    for (int i = 0; i < 256; i++) {
        ((char *)&buf)[i] = (char)i; // writes far past buf
    }
    return 0;
}

void start() {
    int r = victim();
    (void)r;
}
