/* CWE 188: Reliance on Data/Memory Layout */
struct Pair {
    int a;
    int b;
};

static struct Pair p = {1, 2};
volatile int sink;

void start(void) {
    int *raw = (int *)&p;
    /* assume a,b are contiguous and blindly write two ints */
    raw[0] = 10;
    raw[1] = 20;
    raw[2] = 30; /* overwrite beyond struct */
    sink = p.a + p.b;
}
