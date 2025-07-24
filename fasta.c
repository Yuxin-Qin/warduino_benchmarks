// fasta_cheri_benchmark.c
#define NULL 0
#define MALLOC my_malloc
#define FREE my_free
#define INIT_MEM init_mem_pool()

#define MAXIMUM_LINE_WIDTH 60
#define FIXED_N 25000000

#define CELL_SIZE 64
#define POOL_SIZE_IN_PAGES 2000
#define PAGE_SIZE (1 << 12)
char mem[POOL_SIZE_IN_PAGES * PAGE_SIZE];

typedef intptr_t intnative_t;

typedef struct {
    char letter;
    float probability;
} nucleotide_info;

typedef union {
    char bytes[CELL_SIZE];
    void *ptr;
} Cell;

void *pool = NULL;
Cell *freelist = NULL;
int total_allocs = 0;
int total_frees = 0;

void print_string(const char *s);
void print_int(int n);

void init_mem_pool() {
    void *p = &mem[0];
    unsigned int pool_size = POOL_SIZE_IN_PAGES * PAGE_SIZE;
    Cell *cell = (Cell *)p;
    while ((uintptr_t)cell < ((uintptr_t)p + pool_size - CELL_SIZE)) {
        Cell *next = cell + 1;
        cell->ptr = next;
        cell = next;
    }
    cell->ptr = NULL;
    freelist = (Cell *)p;
    pool = p;
}

void *my_malloc(unsigned int num_bytes) {
    void *p;
    if (freelist == NULL) {
        p = NULL;
    } else {
        p = (void *)freelist;
        freelist = freelist->ptr;
        total_allocs++;
    }
    return p;
}

void my_free(void *ptr) {
    if (!ptr) return;
    Cell *empty = (Cell *)ptr;
    empty->ptr = freelist;
    freelist = empty;
    total_frees++;
}

// Deterministic RNG
#define IM 139968
#define IA 3877
#define IC 29573
uint32_t seed = 42;

static inline uint32_t next_rand() {
    seed = (seed * IA + IC) % IM;
    return seed;
}

void repeat_And_Wrap_String(const char *str, int n) {
    int len = strlen(str);
    int total = n;
    int offset = 0;

    char line[MAXIMUM_LINE_WIDTH + 1];
    line[MAXIMUM_LINE_WIDTH] = '\n';

    while (total > 0) {
        int w = (total < MAXIMUM_LINE_WIDTH) ? total : MAXIMUM_LINE_WIDTH;
        for (int i = 0; i < w; i++) {
            line[i] = str[offset];
            offset = (offset + 1) % len;
        }
        print_string(line);
        total -= w;
    }
}

void generate_DNA(const nucleotide_info *info, int count, int n) {
    uint32_t probs[count];
    float acc = 0.0;
    for (int i = 0; i < count; i++) {
        acc += info[i].probability;
        probs[i] = 1 + (uint32_t)(acc * IM);
    }

    int col = 0;
    for (int i = 0; i < n; i++) {
        uint32_t r = next_rand();
        int j = 0;
        while (j < count - 1 && r >= probs[j]) j++;
        print_int((int)info[j].letter);  // Replace with `putchar(info[j].letter);` if using stdout
        col++;
        if (col == MAXIMUM_LINE_WIDTH) {
            print_string("\n");
            col = 0;
        }
    }
    if (col != 0) print_string("\n");
}

void start() {
    INIT_MEM;

    print_string(">ONE Homo sapiens alu\n");
    const char alu[] =
        "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTC"
        "AGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCG"
        "TGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGG"
        "AGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

    repeat_And_Wrap_String(alu, 2 * FIXED_N);

    print_string(">TWO IUB ambiguity codes\n");
    nucleotide_info iub[] = {
        {'a', 0.27}, {'c', 0.12}, {'g', 0.12}, {'t', 0.27},
        {'B', 0.02}, {'D', 0.02}, {'H', 0.02}, {'K', 0.02},
        {'M', 0.02}, {'N', 0.02}, {'R', 0.02}, {'S', 0.02},
        {'V', 0.02}, {'W', 0.02}, {'Y', 0.02}
    };
    generate_DNA(iub, sizeof(iub) / sizeof(nucleotide_info), 3 * FIXED_N);

    print_string(">THREE Homo sapiens frequency\n");
    nucleotide_info hs[] = {
        {'a', 0.3029549426680}, {'c', 0.1979883004921},
        {'g', 0.1975473066391}, {'t', 0.3015094502008}
    };
    generate_DNA(hs, sizeof(hs) / sizeof(nucleotide_info), 5 * FIXED_N);

    print_string("Allocations: ");
    print_int(total_allocs);
    print_string(", Frees: ");
    print_int(total_frees);
    print_string("\n");
}
