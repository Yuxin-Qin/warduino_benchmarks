/* Region-level bug: write from region A into region B */

volatile int regionA[16];   /* region A: 16 ints  */
volatile int regionB[16];   /* region B: 16 ints  */
volatile int guardA = 111;
volatile int guardB = 222;

void start(void) {
    volatile int *p = regionA;

    /* Legit: fill regionA[0..15] */
    for (int i = 0; i < 16; i++) {
        p[i] = i;
    }

    /* Region-level overflow:
       p[16]..p[31] walk past regionA into regionB (and beyond).
       On non-CHERI: typically just corrupts regionB / guards.
       On CHERI: capability for regionA should bound this, so
       the first access beyond regionA triggers a capability fault. */
    for (int i = 16; i < 32; i++) {
        p[i] = 0xCAFEBABE;   /* crosses region boundary */
    }

    /* Use guards so optimiser can't remove them */
    if (guardA == 0 || guardB == 0) {
        /* unreachable, but keeps variables “live” */
        __asm__ volatile ("" ::: "memory");
    }
}
