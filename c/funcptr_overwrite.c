// Intentional memory weakness: overwrite of function pointer via nearby buffer

typedef int (*fn_ptr)(void);

static int safe_func(void) {
    return 1;
}

static int evil_func(void) {
    // never intended to be called in a safe program
    return 42;
}

void start() {
    volatile char buf[16];
    volatile fn_ptr fp = safe_func;

    // Deliberately scribble into memory near fp.
    // On some layouts this can overwrite fp (undefined behaviour).
    for (int i = 0; i < 64; i++) {
        ((char *)&buf)[i] = (char)i;
    }

    // Call through possibly corrupted function pointer
    int result = fp();
    (void)result;
}
