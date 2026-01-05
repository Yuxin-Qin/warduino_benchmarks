#include "wasm_layout.h"

/*
 * CWE-121:
 * Build a stack log message and append a tag,
 * but never check the destination size.
 */
void start(void) {
    char log_buf[32];
    char tag[48];
    int  acc = 0;

    for (int i = 0; i < 32; i++) {
        log_buf[i] = 'A';
    }
    for (int i = 0; i < 48; i++) {
        tag[i] = (char)('a' + (i % 26));
    }

    /* Overflow log_buf. */
    for (int i = 0; i < 48; i++) {
        log_buf[i] = tag[i];
    }

    for (int i = 0; i < 32; i++) {
        acc += (int)log_buf[i];
    }

    print_int(acc);
}
