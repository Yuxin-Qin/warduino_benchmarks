/* CWE-786 */
volatile int sink;
void start(void) {
    int buf[8];
    int *p = &buf[0];
    p -= 2;         /* move before buffer */
    *p = 123;       /* write before start */
    sink = buf[0];
}

