#include "wasm_layout.h"

/*
 * CWE-127:
 * Sliding read window index goes negative conceptually.
 */
void start(void) {
    unsigned char buf[32];
    int           start_index = 8;
    int           sum         = 0;

    for (int i = 0; i < 32; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    start_index -= 16;  /* becomes negative logically */

    for (int i = start_index; i < start_index + 10; i++) {
        sum += buf[i];  /* under-read for i < 0 in C semantics */
    }

    print_int(sum);
}
