/* CWE-121 variant 7 */
volatile int guard = 12345;
void start(void) {
    int buf[8];
    for (int j = 0; j < 8; j++) {
        buf[j] = j;
    }
    /* Overwrite past stack buffer; may smash guard or return addr */
    for (int j = 0; j < 8 + 7; j++) {
        buf[j] = 0x11110000 + j;
    }
    (void)guard;
}
