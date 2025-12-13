/* CWE-119 */
volatile int sink;
void start(void) {
    char buf[16];
    int len = 24;  /* overly permissive length */
    for (int i = 0; i < len; i++) {
        buf[i] = (char)i;  /* may exceed buf[15] */
    }
    sink = buf[0];
}

