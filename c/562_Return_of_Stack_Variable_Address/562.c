/* CWE-562 */
volatile int sink;
int *get_local(void) {
    int local[4];
    local[1] = 10;
    return &local[1];  /* returns stack address */
}
void start(void) {
    int *p = get_local();
    sink = *p;  /* invalid dereference */
}

