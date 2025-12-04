/* CWE 123: Write-what-where Condition */

volatile int sink;

void start(void) {
    int target = 1;
    int other  = 2;

    int *where = &target;
    int *p = &other;

    /* Corrupt "where" via invalid offset computation */
    where = (int *)((char *)where + sizeof(int) * 2); /* points beyond */

    /* Now write to unintended location */
    *where = *p;

    sink = target;
}
