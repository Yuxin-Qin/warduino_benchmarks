// Intentional memory weakness: stale pointer into a local array
static volatile char *saved;

static void make_pointer(void) {
    volatile char local[8];
    for (int i = 0; i < 8; i++) {
        ((char *)local)[i] = (char)i;
    }
    saved = (char *)local; // pointer to stack memory that will go out of scope
}

void start() {
    make_pointer();
    // Use stale pointer after frame is gone
    saved[0] = 42;
}
