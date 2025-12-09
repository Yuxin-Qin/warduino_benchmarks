volatile int sink;

static int buffer[128];

void start(void) {
    /* Define a logical window of length 8 inside buffer */
    int *window = &buffer[32];
    int logical_len = 8;
    int i;

    for (i = 0; i < logical_len; i++) {
        window[i] = i + 10;
    }

    /* Incorrect offset: write at index 12 instead of < 8.
       Still within 'buffer', but outside logical window. */
    int *p = window + 12;
    *p = 777;

    sink = window[0];
}
