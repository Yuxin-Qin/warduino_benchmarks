/* e.g. 787_Out_of_bounds_Write.c */

volatile int sink;   /* to prevent dead-code elimination */

void start(void) {
    /* Size deliberately tiny and not a multiple of 64 KiB. */
    int buf[8];
    int neighbour[8];

    /* CHERI: capability for buf is bounded to buf[0..7].      */
    /* Non-CHERI: buf & neighbour are just addresses in linear */
    /* memory; writing buf[8..15] clobbers neighbour.          */

    for (int i = 0; i < 16; i++) {
        buf[i] = i;    /* i >= 8 is OOB on buf */
    }

    /* Prevent optimisation away. */
    sink = neighbour[0];
}
