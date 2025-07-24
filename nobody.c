#define NULL 0

/**************
 * Memory Allocator (Fixed-size Cell Pool)
 */
#define CELL_SIZE 64
typedef union {
  char bytes[CELL_SIZE];
  void *ptr;
} Cell;

#define POOL_SIZE_IN_PAGES 2000
#define PAGE_SIZE (1 << 12)

char mem[POOL_SIZE_IN_PAGES * PAGE_SIZE];
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
 * Fixed-Point N-Body Benchmark
 */
#define NUM_BODIES 3
#define STEPS 50000000
#define FP_SCALE 1000

typedef struct {
  int x[3];  // position scaled by FP_SCALE
  int v[3];  // velocity
  int mass;
} Body;

void zero_velocity(Body *bodies) {
  for (int k = 0; k < 3; ++k)
    for (int i = 1; i < NUM_BODIES; ++i)
      bodies[0].v[k] -= (bodies[i].v[k] * bodies[i].mass) / bodies[0].mass;
}

void advance(Body *bodies) {
  for (int i = 0; i < NUM_BODIES; ++i) {
    for (int j = i + 1; j < NUM_BODIES; ++j) {
      int dx = bodies[i].x[0] - bodies[j].x[0];
      int dy = bodies[i].x[1] - bodies[j].x[1];
      int dz = bodies[i].x[2] - bodies[j].x[2];
      int dist = dx * dx + dy * dy + dz * dz + 1;

      int f = FP_SCALE / dist;  // fake inverse-square gravity
      for (int k = 0; k < 3; ++k) {
        int dv = (bodies[j].x[k] - bodies[i].x[k]) * f;
        bodies[i].v[k] += dv * bodies[j].mass / (FP_SCALE * 10);
        bodies[j].v[k] -= dv * bodies[i].mass / (FP_SCALE * 10);
      }
    }
  }

  for (int i = 0; i < NUM_BODIES; ++i)
    for (int k = 0; k < 3; ++k)
      bodies[i].x[k] += bodies[i].v[k];
}

void print_state(Body *bodies) {
  for (int i = 0; i < NUM_BODIES; ++i) {
    print_string("Body "); print_int(i); print_string(": ");
    for (int k = 0; k < 3; ++k) {
      print_int(bodies[i].x[k]); print_string(" ");
    }
    print_string("\n");
  }
}


/**************
 * Benchmark Entrypoint
 */
void start() {
  print_string("N-Body (fixed-point) benchmark\n");
  init_mem_pool();

  Body *bodies = (Body *)my_malloc(sizeof(Body) * NUM_BODIES);

  // Initial conditions
  bodies[0] = (Body){{0, 0, 0}, {0, 0, 0}, 10000};
  bodies[1] = (Body){{FP_SCALE, 0, 0}, {0, 2, 0}, 1};
  bodies[2] = (Body){{-FP_SCALE, 0, 0}, {0, -2, 0}, 1};

  zero_velocity(bodies);

  for (int i = 0; i < STEPS; ++i) {
    if (i % 10000000 == 0) {
      print_string("Step "); print_int(i); print_string("\n");
    }
    advance(bodies);
  }

  print_state(bodies);
}
