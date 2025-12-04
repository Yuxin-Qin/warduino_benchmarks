/* CWE 134: Use of Externally-Controlled Format String (simulated, no I/O) */

volatile int sink;

void start(void) {
    char fmt[16];
    char user[32];
    int i;

    /* simulate attacker-controlled string with % specifiers */
    for (i = 0; i < 16; i++) {
        fmt[i] = '%';
    }
    for (i = 0; i < 32; i++) {
        user[i] = (char)(i + 33);
    }

    /* No real printf here due to nostdlib; treat as misuse of untrusted layout */
    sink = fmt[0] + user[0];
}
