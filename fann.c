#define NULL 0

/**************
 * Memory Allocator
 */
#define CELL_SIZE 64
typedef union {
  char bytes[CELL_SIZE];
  void *ptr;
} Cell;

#define POOL_SIZE_IN_PAGES 2000
#define PAGE_SIZE (1 << 12)

char mem[POOL_SIZE_IN_PAGES * PAGE_SIZE];
void *pool = NULL;
Cell *freelist = NULL;

void init_mem_pool() {
  void *p = &mem[0];
  unsigned int pool_size = POOL_SIZE_IN_PAGES * PAGE_SIZE;
  Cell *cell = (Cell *)p;
  while ((char *)cell < ((char *)p + pool_size - CELL_SIZE)) {
    cell->ptr = cell + 1;
    cell++;
  }
  cell->ptr = NULL;
  freelist = (Cell *)p;
  pool = p;
}

void *my_malloc(unsigned int num_bytes) {
  if (freelist == NULL) return NULL;
  void *p = (void *)freelist;
  freelist = freelist->ptr;
  return p;
}

void my_free(void *ptr) {
  Cell *empty = (Cell *)ptr;
  empty->ptr = freelist;
  freelist = empty;
}

/**************
 * Minimal Print Interface
 */
void print_nl() {
  print_string("\n");
}

/**************
 * Fannkuch Benchmark
 */
#define N 2  // maximum N is 11 due to 64-bit factorial
#define FACTORIAL_SIZE (N+1)

void start() {
  print_string("fannkuch start\n");
  init_mem_pool();

  int *perm = (int *)my_malloc(sizeof(int) * N);
  int *perm1 = (int *)my_malloc(sizeof(int) * N);
  int *count = (int *)my_malloc(sizeof(int) * N);
  int maxflips = 0, checksum = 0;
  int i;

  for (i = 0; i < N; ++i) perm1[i] = i;

  int r = N;
  while (1) {
    for (i = 0; i < N; ++i) perm[i] = perm1[i];
    int flips = 0;
    while (perm[0] != 0) {
      int k = perm[0];
      for (int i = 0, j = k; i < j; ++i, --j) {
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
      }
      flips++;
    }
    if (flips > maxflips) maxflips = flips;
    checksum += (r % 2 == 0) ? flips : -flips;

    while (r != 1) {
      count[r - 1] = r;
      r--;
    }

    while (1) {
      if (r == N) {
        print_string("checksum: "); print_int(checksum); print_nl();
        print_string("max flips: "); print_int(maxflips); print_nl();
        return;
      }
      int perm0 = perm1[0];
      for (i = 0; i < r; ++i) perm1[i] = perm1[i + 1];
      perm1[r] = perm0;
      count[r]++;
      if (count[r] <= r) break;
      count[r] = 0;
      r++;
    }
  }
}
