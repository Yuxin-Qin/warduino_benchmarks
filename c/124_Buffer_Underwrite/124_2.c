#include "wasm_layout.h"

/*
 * CWE-124:
 * Sliding window cursor moved too far backward, then written.
 */
void start(void) {
    unsigned char window[32];
    unsigned char *cursor = &window[8];
    int sum = 0;

    for (int i = 0; i < 32; i++) {
        window[i] = (unsigned char)i;
    }

    cursor -= 16;  /* before window[0] */
    for (int i = 0; i < 8; i++) {
        cursor[i] = (unsigned char)(i + 100);
    }

    for (int i = 0; i < 32; i++) {
        sum += window[i];
    }

    print_int(sum);
}
