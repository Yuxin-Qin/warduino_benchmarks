/* CWE-680 */
volatile int sink;
void start(void) {
    unsigned int count = 0x40000000u;
    unsigned int size = count * 8;  /* overflow */
    char buf[32];
    for (unsigned int i = 0; i < size; i++) {
        buf[i] = (char)i;  /* overflow buffer */
    }
    sink = buf[0];
}

