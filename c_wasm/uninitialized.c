/*
 * Use an uninitialized local variable.
 */
volatile int sink;

int main(void) {
    int x; /* uninitialized */
    sink = x; /* use without initialization */
    return 0;
}
