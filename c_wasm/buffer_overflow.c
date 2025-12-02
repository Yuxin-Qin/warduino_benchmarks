/*
 * Intentional stack buffer overflow:
 * write 16 bytes into an 8-byte array.
 * No headers, no libc.
 */
int main(void) {
    char buf[8];
    int i;
    for (i = 0; i < 16; ++i) {
        buf[i] = (char)i; /* out-of-bounds writes */
    }
    return (int)buf[0];
}
