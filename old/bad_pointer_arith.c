// bad_pointer_arith.c
#include <stdio.h>

int main(void) {
    int arr[10];
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }

    int *p = arr;
    p += 12;            // BUG: points well past the array
    *p = 42;            // UB write

    printf("arr[0] = %d\n", arr[0]);
    return 0;
}

