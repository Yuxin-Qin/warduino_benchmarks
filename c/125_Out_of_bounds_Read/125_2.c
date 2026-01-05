#include "wasm_layout.h"

/*
 * CWE-125:
 * Scan payload for sentinel but ignore declared length.
 */
void start(void) {
    unsigned char payload[32];
    int           len   = 16;
    int           found = -1;

    for (int i = 0; i < 32; i++) {
        payload[i] = (unsigned char)(i & 0xff);
    }
    payload[10] = 0xAA;

    for (int i = 0; i < 32; i++) {  /* ignores len */
        if (payload[i] == 0xAA) {
            found = i;
            break;
        }
    }

    print_int(found);
}
