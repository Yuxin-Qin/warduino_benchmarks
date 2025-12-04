/* CWE 244: Improper Clearing of Heap Memory Before Release (simulated) */

volatile int sink;
static char heap_area[32];

void fake_free(char *p, int n) {
    int i;
    /* failure to clear sensitive data; here, we just leave it */
    for (i = 0; i < n; i++) {
        sink += p[i]; /* "leak" via side-channel sink */
    }
}

void start(void) {
    char *p = heap_area;
    int i;

    for (i = 0; i < 32; i++) {
        p[i] = (char)(i + 1); /* pretend secret data */
    }

    fake_free(p, 32);
}
