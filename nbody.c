// WARDuino-compatible CHERI-safe malloc version of N-Body benchmark
// No stdio, no dynamic memory allocation. Uses WARDuino print primitives.

#define PI 3.141592653589793
#define SOLAR_MASS (4 * PI * PI)
#define DAYS_PER_YEAR 365.24

#define NULL 0
#define CELL_SIZE 64
#define POOL_SIZE_IN_PAGES 2000
#define PAGE_SIZE (1 << 12)

// ------------------ Print Primitives ------------------
void print_string(const char *s);
void print_int(int n);
void print_double(double d);

void print_double(double x) {
  int scaled = (int)(x * 1e9);
  print_int(scaled);
  print_string(" (x1e-9)\n");
}

// ------------------ Memory Allocator ------------------
typedef union {
  char bytes[CELL_SIZE];
  void *ptr;
} Cell;

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
  if (!ptr) return;
  Cell *empty = (Cell *)ptr;
  empty->ptr = freelist;
  freelist = empty;
}

// ------------------ N-Body Simulation ------------------
typedef struct {
  double x[3], v[3], mass;
} Body;

#define BODIES_COUNT 5

Body *allocate_body() {
  Body *b = (Body *)my_malloc(sizeof(Body));
  return b;
}

void offset_momentum(Body **bodies, int n) {
  for (int i = 0; i < n; i++)
    for (int k = 0; k < 3; k++)
      bodies[0]->v[k] -= bodies[i]->v[k] * bodies[i]->mass / SOLAR_MASS;
}

void bodies_advance(Body **bodies, int n, double dt) {
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      double dx[3];
      for (int k = 0; k < 3; k++)
        dx[k] = bodies[i]->x[k] - bodies[j]->x[k];

      double dist_sqr = dx[0]*dx[0] + dx[1]*dx[1] + dx[2]*dx[2];
      double dist = dist_sqr;
      for (int k = 0; k < 10; k++) dist = 0.5 * (dist + dist_sqr / dist);
      double mag = dt / (dist_sqr * dist);

      for (int k = 0; k < 3; k++) {
        double d = dx[k] * mag;
        bodies[i]->v[k] -= d * bodies[j]->mass;
        bodies[j]->v[k] += d * bodies[i]->mass;
      }
    }
  }
  for (int i = 0; i < n; i++)
    for (int k = 0; k < 3; k++)
      bodies[i]->x[k] += dt * bodies[i]->v[k];
}

double bodies_energy(Body **bodies, int n) {
  double e = 0.0;
  for (int i = 0; i < n; i++) {
    double v2 = bodies[i]->v[0]*bodies[i]->v[0] +
                bodies[i]->v[1]*bodies[i]->v[1] +
                bodies[i]->v[2]*bodies[i]->v[2];
    e += 0.5 * bodies[i]->mass * v2;
    for (int j = i + 1; j < n; j++) {
      double dx[3];
      for (int k = 0; k < 3; k++) dx[k] = bodies[i]->x[k] - bodies[j]->x[k];
      double dist = dx[0]*dx[0] + dx[1]*dx[1] + dx[2]*dx[2];
      for (int k = 0; k < 10; k++) dist = 0.5 * (dist + dx[0]*dx[0] / dist);
      e -= (bodies[i]->mass * bodies[j]->mass) / dist;
    }
  }
  return e;
}

void start() {
  init_mem_pool();

  Body **bodies = (Body **)my_malloc(sizeof(Body *) * BODIES_COUNT);
  for (int i = 0; i < BODIES_COUNT; i++) bodies[i] = allocate_body();

  // Initialize bodies
  *bodies[0] = (Body){{0, 0, 0}, {0, 0, 0}, SOLAR_MASS};
  *bodies[1] = (Body){{4.84143144246472090, -1.16032004402742839, -0.103622044471123109},
                     {0.00166007664274403694 * DAYS_PER_YEAR,
                      0.00769901118419740425 * DAYS_PER_YEAR,
                      -0.0000690460016972063023 * DAYS_PER_YEAR},
                     0.000954791938424326609 * SOLAR_MASS};
  *bodies[2] = (Body){{8.34336671824457987, 4.12479856412430479, -0.403523417114321381},
                     {-0.00276742510726862411 * DAYS_PER_YEAR,
                      0.00499852801234917238 * DAYS_PER_YEAR,
                      0.0000230417297573763929 * DAYS_PER_YEAR},
                     0.000285885980666130812 * SOLAR_MASS};
  *bodies[3] = (Body){{12.8943695621391310, -15.1111514016986312, -0.223307578892655734},
                     {0.00296460137564761618 * DAYS_PER_YEAR,
                      0.00237847173959480950 * DAYS_PER_YEAR,
                      -0.0000296589568540237556 * DAYS_PER_YEAR},
                     0.0000436624404335156298 * SOLAR_MASS};
  *bodies[4] = (Body){{15.3796971148509165, -25.9193146099879641, 0.179258772950371181},
                     {0.00268067772490389322 * DAYS_PER_YEAR,
                      0.00162824170038242295 * DAYS_PER_YEAR,
                      -0.0000951592254519715870 * DAYS_PER_YEAR},
                     0.0000515138902046611451 * SOLAR_MASS};

  offset_momentum(bodies, BODIES_COUNT);

  print_string("Initial energy:\n");
  print_double(bodies_energy(bodies, BODIES_COUNT));

  for (int i = 0; i < 1000; i++)
    bodies_advance(bodies, BODIES_COUNT, 0.01);

  print_string("Final energy:\n");
  print_double(bodies_energy(bodies, BODIES_COUNT));
}
