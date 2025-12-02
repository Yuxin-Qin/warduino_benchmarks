/*
 * Overwrite a function pointer via buffer overflow.
 */

typedef void (*func_t)(void);

static int flag = 0;

static void safe_function(void) {
    flag = 1;
}

static void evil_function(void) {
    flag = 2;
}

int main(void) {
    char buf[16];
    func_t fp = safe_function;

    /* Layout: buf[] followed by fp on the stack (approximate).
       The exact placement is implementation-dependent, but this
       pattern captures the idea of smashing the function pointer. */
    int i;
    for (i = 0; i < 32; ++i) {
        buf[i] = (char)((unsigned long)evil_function >> (i % 8));
    }

    fp(); /* potentially corrupted */
    return flag;
}
