// retaddr_overwrite.c
#include <stdio.h>
#include <string.h>

void vulnerable(const char *input) {
    char buf[32];

    // BUG: no bounds check, possible return-address overwrite
    strcpy(buf, input);

    printf("vulnerable() saw: %.32s\n", buf);
}

int main(void) {
    char payload[128];
    memset(payload, 'A', sizeof(payload));
    payload[sizeof(payload) - 1] = '\0';

    vulnerable(payload);
    printf("If you see this, we did not crash (yet).\n");
    return 0;
}
