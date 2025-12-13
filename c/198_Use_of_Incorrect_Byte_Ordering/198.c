/* CWE-198 */
volatile int sink;
void start(void) {
    unsigned char bytes[4];
    bytes[0] = 1;
    bytes[1] = 2;
    bytes[2] = 3;
    bytes[3] = 4;
    /* Misinterpreted endianness */
    unsigned int v = ((unsigned int)bytes[0] << 24) |
                     ((unsigned int)bytes[1] << 16) |
                     ((unsigned int)bytes[2] << 8)  |
                     ((unsigned int)bytes[3]);
    sink = (int)v;
}

