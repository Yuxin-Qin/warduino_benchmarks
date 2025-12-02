// use_after_free.c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));
    if (!p) return 1;
    *p = 123;

    free(p);
    // BUG: use pointer after it is freed
    printf("Use-after-free value: %d\n", *p);

    return 0;
}
