/* CWE 188: Reliance on Data/Memory Layout */

volatile int sink;

struct Pair {
    int x;
    int y;
};

void start(void) {
    struct Pair p;
    int *raw = (int *)&p;

    raw[0] = 1;
    raw[1] = 2;

    /* Assume that "y" immediately follows "x" and manipulate via index */
    raw[2] = 3; /* out-of-struct write */

    sink = p.x + p.y;
}
