/*
 * Misusing struct layout by reinterpreting memory
 * as a different struct type.
 */
struct A {
    int a;
    int b;
};

struct B {
    int x;
    int y;
    int z;
};

volatile int sink;

int main(void) {
    struct A obj = {1, 2};
    struct B *p = (struct B *)&obj; /* incorrect layout assumption */
    sink = p->z; /* access field that does not exist */
    return 0;
}
