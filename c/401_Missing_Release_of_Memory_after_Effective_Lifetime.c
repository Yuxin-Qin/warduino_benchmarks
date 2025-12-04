/* CWE 401: Missing Release of Memory after Effective Lifetime (leak) */

volatile int sink;
static char heap_area[256];
static int heap_index;

char *alloc_block(int n) {
    char *p;
    if (heap_index + n > 256) {
        return 0;
    }
    p = &heap_area[heap_index];
    heap_index += n;
    return p;
}

void start(void) {
    char *blocks[8];
    int i, j;

    heap_index = 0;

    for (i = 0; i < 8; i++) {
        blocks[i] = alloc_block(24);
        if (!blocks[i]) {
            break;
        }
        for (j = 0; j < 24; j++) {
            blocks[i][j] = (char)(i + j);
        }
    }

    /* never free blocks -> leak within artificial heap */
    for (i = 0; i < 8; i++) {
        if (blocks[i]) {
            sink += blocks[i][0];
        }
    }
}
