#include "wasm_layout.h"

/*
 * CWE-131:
 * Mis-sized path buffer; no room for full string.
 */
void start(void) {
    char base[8]   = {'/','a','p','p','/',0,0,0};
    char suffix[8] = {'l','o','g','.','t','x','t',0};
    char path[12];
    int  i, idx = 0;
    int  acc = 0;

    for (i = 0; i < 8 && base[i] != 0; i++) {
        path[idx++] = base[i];
    }
    for (i = 0; i < 8 && suffix[i] != 0; i++) {
        path[idx++] = suffix[i];  /* may overflow path */
    }

    for (i = 0; i < 12; i++) {
        acc += (int)path[i];
    }

    print_int(acc);
}
