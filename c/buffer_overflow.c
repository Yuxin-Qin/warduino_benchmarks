// Intentional memory weakness: stack buffer overflow
void start() {
    volatile char buf[8];
    // Write past the end of buf
    for (int i = 0; i < 16; i++) {
        buf[i] = (char)i;
    }
}
