/* CWE 131: Incorrect Calculation of Buffer Size */
static char buf[16];
volatile int sink;

void start(void) {
    int logical_len = 16;
    int wrong_size = logical_len + 8; /* miscalculation */

    int i;
    for (i = 0; i < wrong_size; i++) {
        buf[i] = (char)i; /* overwrite past end */
    }

    sink = buf[0];
}
