#include "wasm_layout.h"

static int process_payload(unsigned char *buf, unsigned long len) {
    int sum = 0;
    for (unsigned long i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}

/*
 * CWE-805:
 * Caller passes length too large; callee trusts it.
 */
void start(void) {
    unsigned char buf[32];
    unsigned long real_len  = 16;
    unsigned long wrong_len = 64;
    (void)real_len;

    for (int i = 0; i < 32; i++) {
        buf[i] = (unsigned char)(i & 0xff);
    }

    int result = process_payload(buf, wrong_len);
    print_int(result);
}
