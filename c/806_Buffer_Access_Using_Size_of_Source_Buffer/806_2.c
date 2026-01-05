#include "wasm_layout.h"

/*
 * CWE-806:
 * Uses size of src when copying into smaller dst.
 */
void start(void) {
    unsigned char src[64];
    unsigned char dst[16];
    int           sum = 0;

    for (int i = 0; i < 64; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    for (int i = 0; i < 64; i++) {  /* should cap at 16 */
        dst[i] = src[i];
    }

    for (int i = 0; i < 16; i++) {
        sum += dst[i];
    }

    print_int(sum);
}
