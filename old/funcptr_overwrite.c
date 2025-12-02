// funcptr_overwrite_demo.c (safer demo version)
#include <stdio.h>
#include <string.h>

typedef void (*handler_fn)(void);

void safe_handler(void)  { printf("Safe handler called.\n"); }
void evil_handler(void)  { printf("EVIL handler executed! (simulated hijack)\n"); }

struct Device {
    char name[8];
    handler_fn handler;
};

int main(void) {
    struct Device dev;
    strcpy(dev.name, "sensor");
    dev.handler = safe_handler;

    printf("Before overflow: handler = safe_handler\n");

    // Simulate overflow: attacker manages to write address of evil_handler
    // into handler field via a bug in the real program.
    dev.handler = evil_handler;  // in practice, this would come from overflow

    printf("After overflow: calling dev.handler...\n");
    dev.handler();
    return 0;
}
