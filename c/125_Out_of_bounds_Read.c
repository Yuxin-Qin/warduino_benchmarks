/* CWE 125: Out-of-bounds Read */
static char buf[16] = {
    1,2,3,4,5,6,7,8,
    9,10,11,12,13,14,15,16
};
volatile int sink;

void start(void) {
    int i;
    int sum = 0;

    for (i = 0; i < 32; i++) {
        sum += buf[i]; /* reads past buf[15] */
    }

    sink = sum;
}
