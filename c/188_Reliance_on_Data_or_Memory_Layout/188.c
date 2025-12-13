/* CWE-188 */
volatile int sink;
struct S { char a; int b; char c; };
void start(void) {
    struct S s;
    s.a = 1;
    s.b = 0x22223333;
    s.c = 3;
    char *p = (char *)&s.a;
    int *pb = (int *)(p + 1); /* assumes no padding */
    sink = *pb;
}

