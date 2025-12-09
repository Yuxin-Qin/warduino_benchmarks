volatile int sink;

static int global_arena[256];

void start(void) {
    /* Sub-region advertised to caller as length 32 starting at 64. */
    int *sub = &global_arena[64];
    int advertised_len = 32;
    int i;

    for (i = 0; i < advertised_len; i++) {
        sub[i] = 700 + i;
    }

    /* Caller miscalculates length and writes beyond end of sub-region. */
    int wrong_len = advertised_len + 16;
    int *p = sub + wrong_len;  /* still inside global_arena */
    *p = 0x6666;

    sink = sub[0];
}
