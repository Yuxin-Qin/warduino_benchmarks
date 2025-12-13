/* CWE-127 */
volatile int sink;
void start(void) {
    char buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (char)(i + 1);
    char *p = &buf[2];
    char v = p[-4];  /* read before buf[0] */
    sink = (int)v;
}

