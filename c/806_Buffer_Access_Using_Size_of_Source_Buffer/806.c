/* CWE-806 */
volatile int sink;
void start(void) {
    char src[16];
    char dst[8];
    for (int i = 0; i < 16; i++) src[i] = (char)i;
    int len = sizeof(src);  /* uses source size */
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];   /* overflow dst */
    }
    sink = dst[0];
}

