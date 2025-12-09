volatile int sink;

struct Block {
    int header[4];   /* meta-data */
    int payload[8];  /* logical region */
    int trailer[4];  /* padding */
};

static struct Block blk;

void start(void) {
    int *payload = blk.payload;
    int i;

    for (i = 0; i < 8; i++) {
        payload[i] = 300 + i;
    }

    /* Bug: code thinks payload has length 12 and writes into trailer. */
    int *p = payload + 10;  /* crosses into blk.trailer */
    *p = 0x3333;

    sink = blk.payload[0];
}
