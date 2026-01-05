#include "wasm_layout.h"

/*
 * CWE-787:
 * Off-by-one log index check.
 */
void start(void) {
    int  log_buf[16];
    int  count = 16;
    int  acc   = 0;

    for (int i = 0; i < 16; i++) {
        log_buf[i] = i;
    }

    if (count <= 16) {
        log_buf[count] = 999;  /* log_buf[16] is out-of-bounds */
        count++;
    }

    for (int i = 0; i < 16; i++) {
        acc += log_buf[i];
    }

    print_int(acc);
}
