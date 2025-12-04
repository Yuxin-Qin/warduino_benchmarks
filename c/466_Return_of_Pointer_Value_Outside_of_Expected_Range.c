/* CWE 466: Return of Pointer Value Outside of Expected Range */

volatile int sink;

char *get_buffer(void) {
    static char buf[16];
    /* incorrect: return pointer to middle, not to start */
    return buf + 8;
}

void start(void) {
    char *p = get_buffer();
    int i;

    for (i = -8; i < 8; i++) {
        p[i] = (char)i; /* p[-8].. underflow the true object start */
    }

    sink = p[0];
}
