// escape_stack.c
#include <stdio.h>

int *return_stack_address(void) {
    static int call_count = 0;
    call_count++;

    int local = 100 + call_count;
    // BUG: returning address of local variable
    return &local;
}

int main(void) {
    int *p = return_stack_address();
    printf("Escaped stack pointer value (UB): %d\n", *p);
    return 0;
}
