#include "wasm_layout.h"

/*
 * CWE-788:
 * Read terminator one past end of buffer.
 */
void start(void) {
    unsigned char buf[16];
    int           acc = 0;

    for (int i = 0; i < 16; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    unsigned char terminator = buf[16];  /* out-of-bounds read */
    acc += (int)terminator;

    for (int i = 0; i < 16; i++) {
        acc += buf[i];
    }

    print_int(acc);
}
