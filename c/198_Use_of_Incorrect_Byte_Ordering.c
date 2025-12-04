/* CWE 198: Use of Incorrect Byte Ordering (simulated) */
static unsigned char bytes[4] = {0x11, 0x22, 0x33, 0x44};
volatile unsigned int sink;

void start(void) {
    /* wrong endianness interpretation, then out-of-bounds tweak */
    unsigned int val =
        ((unsigned int)bytes[3] << 24) |
        ((unsigned int)bytes[2] << 16) |
        ((unsigned int)bytes[1] << 8)  |
        ((unsigned int)bytes[0]);

    bytes[4] = 0xFF; /* out-of-bounds write to emphasize bug */
    sink = val;
}
