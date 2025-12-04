/* CWE 763: Release of Invalid Pointer or Reference (simulated) */

volatile int sink;

void my_free(char *p) {
    sink += (int)(unsigned long)p;
}

void start(void) {
    char *invalid = (char *)0x4u;
    my_free(invalid);
}
