#include "wasm_layout.h"

/*
 * CWE-126:
 * Copy full capacity even though only 'received' bytes are valid.
 */
void start(void) {
    unsigned char src[64];
    unsigned char dst[64];
    int           received = 20;
    int           sum      = 0;

    for (int i = 0; i < 64; i++) {
        src[i] = (unsigned char)(i & 0xff);
    }

    for (int i = 0; i < 64; i++) {  /* should use received */
        dst[i] = src[i];
    }

    for (int i = 0; i < 64; i++) {
        sum += dst[i];
    }

    print_int(sum);
}
