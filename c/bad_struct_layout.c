// Intentional memory weakness: aliasing fields via incorrect struct layout

struct A {
    int x;
    int y;
};

struct B {
    int x;
    char small[2];
};

void start() {
    struct A a;
    a.x = 10;
    a.y = 20;

    // Reinterpret A as B and write into small
    struct B *b = (struct B *)&a;
    b->small[0] = 1;
    b->small[1] = 2;
}
