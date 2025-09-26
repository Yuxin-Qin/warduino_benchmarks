#define MATRIX_SIZE 32
#define NUM_ITERATIONS 10

typedef unsigned int uint32_t;
typedef unsigned long size_t;
typedef unsigned long long uint64_t;

static uint32_t A[MATRIX_SIZE][MATRIX_SIZE];
static uint32_t B[MATRIX_SIZE][MATRIX_SIZE];
static uint32_t C[MATRIX_SIZE][MATRIX_SIZE];

void init_matrices() {
    for (size_t i = 0; i < MATRIX_SIZE; ++i) {
        for (size_t j = 0; j < MATRIX_SIZE; ++j) {
            A[i][j] = (i + j) % 128;
            B[i][j] = (i * j) % 128;
            C[i][j] = 0;
        }
    }
}

void clear_matrix() {
    for (size_t i = 0; i < MATRIX_SIZE; ++i) {
        for (size_t j = 0; j < MATRIX_SIZE; ++j) {
            C[i][j] = 0;
        }
    }
}

void matmul() {
    for (size_t i = 0; i < MATRIX_SIZE; ++i) {
        for (size_t j = 0; j < MATRIX_SIZE; ++j) {
            for (size_t k = 0; k < MATRIX_SIZE; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void print_string(const char* s, int len);  // Assumed external function
void print_int(int val);                    // Assumed external function

void start() {
    init_matrices();

    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        clear_matrix();
        matmul();
    }

    uint64_t checksum = 0;
    for (size_t i = 0; i < MATRIX_SIZE; i += 8) {
        for (size_t j = 0; j < MATRIX_SIZE; j += 8) {
            checksum += C[i][j];
        }
    }

    print_string("Checksum: ", 10);
    print_int((int)checksum);
    print_string("\n", 2);
}
