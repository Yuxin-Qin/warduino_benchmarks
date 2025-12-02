// double_free.c
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));
    if (!p) return 1;
    *p = 42;

    free(p);
    // BUG: double free
    free(p);

    return 0;
}
