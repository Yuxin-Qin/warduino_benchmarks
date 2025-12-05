/* Region-level bug in Wasm linear memory:
 * We have two global regions A and B laid out back-to-back.
 * We then write through a pointer to A beyond its logical bounds
 * and into B's storage.
 *
 * In C this is undefined behaviour (region-level overflow).
 * In Wasm, as long as all writes stay inside the linear memory size,
 * the engine won't trap: it just corrupts B.
 *
 * CHERI in WARDuino today typically sees only one capability for the
 * entire linear memory, so it also cannot trap on this region-level bug.
 */

static volatile int regionA[16];  /* logical region A */
static volatile int regionB[16];  /* logical region B */

static volatile int guardA = 111;
static volatile int guardB = 222;

void start(void) {
    int i;

    /* Initialize both regions in a recognisable way. */
    for (i = 0; i < 16; i++) {
        regionA[i] = i;
        regionB[i] = 1000 + i;
    }

    /* Pointer to the start of region A. */
    volatile int *p = regionA;

    /* Legit writes in A: indices 0..15. */
    for (i = 0; i < 16; i++) {
        p[i] = 0xAAAA0000 + i;
    }

    /* Region-level overflow:
     * p[16..31] walks off the end of regionA into regionB.
     * This violates the C object bounds, but is still within the
     * same Wasm linear memory allocation.
     */
    for (i = 16; i < 32; i++) {
        p[i] = 0xBBBB0000 + i;
    }

    /* Use guards so they stay “live” and to give you something
       you can inspect in a debugger / memory dump if you want. */
    if (regionA[0] == guardA || regionB[0] == guardB) {
        guardA = regionA[1];
        guardB = regionB[1];
    }
}
