/* CWE 198: Use of Incorrect Byte Ordering */

volatile int sink;

void start(void) {
    unsigned char buf[4];
    unsigned int value = 0x01020304u;
    unsigned int reconstructed;
    int i;

    /* Naive store as big-endian */
    buf[0] = (unsigned char)((value >> 24) & 0xffu);
    buf[1] = (unsigned char)((value >> 16) & 0xffu);
    buf[2] = (unsigned char)((value >> 8)  & 0xffu);
    buf[3] = (unsigned char)(value & 0xffu);

    /* Naive load as little-endian */
    reconstructed = 0;
    for (i = 0; i < 4; i++) {
        reconstructed |= ((unsigned int)buf[i]) << (8 * i);
    }

    sink = (int)reconstructed;
}
