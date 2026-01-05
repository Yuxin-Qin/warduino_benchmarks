#include "wasm_layout.h"

/*
 * CWE-120:
 * Copy a "received packet" into a small fixed-size stack buffer,
 * using the packet length directly without checking against dest size.
 */
void start(void) {
    unsigned char packet[64];
    unsigned char dest[16];
    int           input_len = 64;  /* "received" length */
    int           checksum  = 0;

    for (int i = 0; i < 64; i++) {
        packet[i] = (unsigned char)(i & 0xff);
    }

    /* Vulnerable: blindly uses input_len. */
    for (int i = 0; i < input_len; i++) {
        dest[i] = packet[i];  /* overflow dest */
    }

    for (int i = 0; i < 16; i++) {
        checksum += dest[i];
    }

    print_int(checksum);
}
