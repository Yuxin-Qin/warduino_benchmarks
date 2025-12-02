/*
 * Stack smashing pattern that could overwrite a saved return address
 * in a traditional ABI. Here we just model a large overflow.
 */

static int marker = 0;

static void vulnerable(void) {
    char buf[16];
    int i;
    for (i = 0; i < 64; ++i) {
        buf[i] = (char)i; /* writes beyond buf[] */
    }
    marker = 1;
}

int main(void) {
    vulnerable();
    return marker;
}
