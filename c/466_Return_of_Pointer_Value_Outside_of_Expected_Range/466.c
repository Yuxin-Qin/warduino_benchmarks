/* CWE-466 */
volatile int sink;
static int data[8];

int *get_ptr(void) {
    return &data[16];  /* pointer outside array */
}
void start(void) {
    int *p = get_ptr();
    *p = 42;
    sink = data[0];
}

