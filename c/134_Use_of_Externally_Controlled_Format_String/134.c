/* CWE-134 (simulated mini printf) */
volatile int sink;
void my_printf(const char *fmt) {
    int acc = 0;
    for (int i = 0; ; i++) {
        char c = fmt[i];
        if (!c) break;
        if (c == '%') {
            char next = fmt[i+4];  /* over-read using 'specifier' */
            acc += (int)next;
        } else {
            acc += (int)c;
        }
    }
    sink = acc;
}
void start(void) {
    char user_buf[16];
    for (int i = 0; i < 16; i++) user_buf[i] = (i % 4 == 0) ? '%' : 'A';
    my_printf(user_buf);
}

