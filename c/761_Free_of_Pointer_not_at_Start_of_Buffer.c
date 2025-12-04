/* CWE 761: Free of Pointer not at Start of Buffer (simulated) */

volatile int sink;

void my_free(char *p) {
    /* expect pointer to start of buffer; using interior pointer is bad */
    sink += (int)(unsigned long)p;
}

void start(void) {
    char buf[16];
    char *mid = buf + 4;
    my_free(mid); /* not at start */
}
