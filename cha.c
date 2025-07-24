#define NULL 0

/**************
 * Memory Allocator (Same as other benchmarks)
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
 * Chameneos Benchmark
 */
#define MEETINGS 6000000
#define NUM_CREATURES 4

typedef enum { BLUE, RED, YELLOW, FADED } Color;

Color complement(Color a, Color b) {
  if (a == b) return a;
  if ((a == RED && b == BLUE) || (a == BLUE && b == RED)) return YELLOW;
  if ((a == RED && b == YELLOW) || (a == YELLOW && b == RED)) return BLUE;
  if ((a == YELLOW && b == BLUE) || (a == BLUE && b == YELLOW)) return RED;
  return FADED;
}

typedef struct {
  Color color;
  int meetings;
} Chameneos;

Chameneos* arena = NULL;
int meeting_count = 0;

void meet(Chameneos* self, Chameneos* partner) {
  Color new_color = complement(self->color, partner->color);
  self->color = new_color;
  self->meetings++;
}

void print_color(Color c) {
  if (c == BLUE) print_string("blue");
  else if (c == RED) print_string("red");
  else if (c == YELLOW) print_string("yellow");
  else print_string("faded");
}

void print_state(Chameneos* c, int id) {
  print_string("Chameneos "); print_int(id);
  print_string(" final color: "); print_color(c->color);
  print_string(", meetings: "); print_int(c->meetings);
  print_string("\n");
}

void start() {
  init_mem_pool();
  print_string("Initialized memory\n");

  Chameneos* creatures = (Chameneos*)my_malloc(sizeof(Chameneos) * NUM_CREATURES);

  // Init colors
  creatures[0].color = BLUE;
  creatures[1].color = RED;
  creatures[2].color = YELLOW;
  creatures[3].color = RED;

  for (int i = 0; i < NUM_CREATURES; i++)
    creatures[i].meetings = 0;

  for (int i = 0; i < MEETINGS; i++) {
    Chameneos *a = &creatures[i % NUM_CREATURES];
    Chameneos *b = &creatures[(i + 1) % NUM_CREATURES];
    meet(a, b);
  }

  print_string("Finished meetings\n");

  for (int i = 0; i < NUM_CREATURES; i++)
    print_state(&creatures[i], i);
}
