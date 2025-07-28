// CHERI-compatible spectral-norm benchmark (WARDuino)
// Retains structure and logic of original benchmark, with minimal changes for CHERI safety

void print_string(const char *s);
void print_int(int n);

#define N 5500
#define MEM_SIZE 819200

static char memory_pool[MEM_SIZE];
static int mem_offset = 0;

void *my_malloc(unsigned int size) {
  if (mem_offset + size >= MEM_SIZE) return 0;
  void *ptr = &memory_pool[mem_offset];
  mem_offset += size;
  return ptr;
}

void my_free(void *ptr) {
  // no-op in fixed pool
}

// Evaluates A[i][j]
double eval_A(int i, int j) {
  int tmp = i + j;
  return 1.0 / ((tmp * (tmp + 1) / 2) + i + 1);
}

void eval_A_times_u(int n, const double *u, double *tmp) {
  for (int i = 0; i < n; i++) {
    double sum = 0.0;
    for (int j = 0; j < n; j++) {
      sum += eval_A(i, j) * u[j];
    }
    tmp[i] = sum;
  }
}

void eval_At_times_u(int n, const double *u, double *v) {
  for (int i = 0; i < n; i++) {
    double sum = 0.0;
    for (int j = 0; j < n; j++) {
      sum += eval_A(j, i) * u[j];
    }
    v[i] = sum;
  }
}

double eval_AtA_times_u(int n, const double *u, double *tmp, double *v) {
  eval_A_times_u(n, u, tmp);
  eval_At_times_u(n, tmp, v);
  return 0.0; // placeholder to match original loop signature
}

void start() {
  double *u = (double *)my_malloc(sizeof(double) * N);
  double *v = (double *)my_malloc(sizeof(double) * N);
  double *tmp = (double *)my_malloc(sizeof(double) * N);

  if (!u || !v || !tmp) {
    print_string("Allocation failed\n");
    return;
  }

  for (int i = 0; i < N; i++) u[i] = 1.0;

  for (int iter = 0; iter < 10; iter++) {
    eval_AtA_times_u(N, u, tmp, v);
    eval_AtA_times_u(N, v, tmp, u);
  }

  double vBv = 0.0, vv = 0.0;
  for (int i = 0; i < N; i++) {
    vBv += u[i] * v[i];
    vv += v[i] * v[i];
  }

  // Manual sqrt replacement
  double x = vBv / vv;
  double guess = x / 2.0;
  for (int i = 0; i < 10; ++i) guess = 0.5 * (guess + x / guess);
  int scaled_result = (int)(guess * 1e9);

  print_string("\nSpectral norm for N=5500:\n");
  print_int(scaled_result);
  print_string(" (scaled x1e9)\n\n");
}