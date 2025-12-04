/* CWE 123: Write-what-where Condition */
static int target1 = 1;
static int target2 = 2;
volatile int sink;

void start(void) {
    int value = 42;
    int *where;

    /* badly validated index controlling where we write */
    int idx = 1; /* attacker-controlled in real world */
    if (idx == 0) {
        where = &target1;
    } else {
        where = &target2;
    }

    *where = value; /* arbitrary write target */
    sink = target1 + target2;
}
