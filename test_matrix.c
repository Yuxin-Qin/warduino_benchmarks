#define MATRIX_SIZE 256
#define NUM_ITERATIONS 1000

typedef unsigned int uint32_t;
typedef unsigned long size_t;
typedef unsigned long long uint64_t;

static uint32_t A[MATRIX_SIZE][MATRIX_SIZE];
static uint32_t B[MATRIX_SIZE][MATRIX_SIZE];
static uint32_t C[MATRIX_SIZE][MATRIX_SIZE];

void init_matrices() {
    for (size_t i = 0; i < MATRIX_SIZE; ++i) {
        for (size_t j = 0; j < MATRIX_SIZE; ++j) {
            A[i][j] = (i + j) % 256;
            B[i][j] = (i * j) % 256;
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

int main() {
    init_matrices();

    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
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

    return 0;
}
