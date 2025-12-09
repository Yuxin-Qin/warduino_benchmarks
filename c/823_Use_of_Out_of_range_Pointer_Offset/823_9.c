volatile int sink;

static int arena[80];

void start(void) {
    int *header  = &arena[0];   /* [0..7]   */
    int *payload = &arena[8];   /* [8..39]  */
    int i;

    for (i = 0; i < 8; i++) {
        header[i] = 10 + i;
    }
    for (i = 0; i < 32; i++) {
        payload[i] = 800 + i;
    }

    /* Bug: treats payload as 40 elements long, overwriting after it. */
    int *p = payload + 36;
    *p = 0x7777;

    sink = header[0];
}
