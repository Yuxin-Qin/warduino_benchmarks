// bad_struct_layout.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct A {
    char c;
    int  x;   // typically aligned with padding after c
};

int main(void) {
    // BUG: allocate too little space assuming no padding
    size_t wrong_size = sizeof(char) + sizeof(int); // looks OK
    if (wrong_size > sizeof(struct A)) {
        wrong_size = sizeof(struct A) - 1; // force it smaller
    }

    char *buf = malloc(wrong_size);
    if (!buf) return 1;

    struct A *p = (struct A *)buf; // mis-sized backing storage
    p->c = 'Z';
    p->x = 12345;  // likely writes past allocated region

    printf("Struct A: c=%c x=%d (UB)\n", p->c, p->x);
    free(buf);
    return 0;
}
