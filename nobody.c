#define CELL_SIZE 64
#define POOL_SIZE_IN_PAGES 2000
#define PAGE_SIZE (1 << 12)

#define TOTAL_CELLS ((POOL_SIZE_IN_PAGES * PAGE_SIZE) / CELL_SIZE)

typedef struct Cell {
  struct Cell *next;
  char payload[CELL_SIZE - sizeof(void *)];
} Cell;

static char mem[POOL_SIZE_IN_PAGES * PAGE_SIZE] __attribute__((aligned(16)));
static Cell *freelist = NULL;

void init_mem_pool() {
  freelist = (Cell *)mem;
  for (int i = 0; i < TOTAL_CELLS - 1; ++i) {
    freelist[i].next = &freelist[i + 1];
  }
  freelist[TOTAL_CELLS - 1].next = NULL;
}

void *my_malloc(unsigned int num_bytes) {
  if (num_bytes > CELL_SIZE - sizeof(void *)) return NULL; // too large
  if (!freelist) return NULL;
  Cell *p = freelist;
  freelist = freelist->next;
  return (void *)p;
}

void my_free(void *ptr) {
  if (!ptr) return;
  Cell *cell = (Cell *)ptr;
  cell->next = freelist;
  freelist = cell;
}
