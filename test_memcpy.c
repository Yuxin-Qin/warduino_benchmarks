#define ARRAY_SIZE (100 * 1024 * 1024) // 100 MB
#define CHUNK_SIZE 4096               // 4 KB
#define NUM_ITERATIONS 10000

typedef unsigned char uint8_t;
typedef unsigned long size_t;
typedef unsigned long long uint64_t;

void *mem_copy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }
    return dest;
}

void start() {
    static uint8_t src[ARRAY_SIZE];
    static uint8_t dst[ARRAY_SIZE];

    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        src[i] = (uint8_t)(i % 256);
    }

    for (size_t i = 0; i < ARRAY_SIZE; i += CHUNK_SIZE) {
        mem_copy(dst + i, src + i, CHUNK_SIZE);
    }

    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        for (size_t i = 0; i < ARRAY_SIZE; i += CHUNK_SIZE) {
            mem_copy(dst + i, src + i, CHUNK_SIZE);
        }
    }

    uint64_t checksum = 0;
    for (size_t i = 0; i < ARRAY_SIZE; i += 4096) {
        checksum += dst[i];
    }

    print_string("Checksum: ", 10);
    print_int((int)checksum);
    print_string("\n", 2);
}
