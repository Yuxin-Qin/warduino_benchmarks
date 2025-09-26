// binarytrees_cheri_benchmark.c
// Improved benchmark with metrics and configurable depth

#define MALLOC   my_malloc
#define FREE     my_free
#define INIT_MEM init_mem_pool()
#define NULL 0

#include <stdint.h>

#define CELL_SIZE 32
#define POOL_SIZE_IN_PAGES 2000
#define PAGE_SIZE (1 << 12)

char mem[POOL_SIZE_IN_PAGES * PAGE_SIZE];
void *pool = NULL;

typedef union {
  char bytes[CELL_SIZE];
  void *ptr;
} Cell;

Cell *freelist = NULL;
int total_allocs = 0;
int total_frees = 0;

extern void print_string(const char *s, int len);
extern void print_int(int n);

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

void *allocate_node(int size) {
  char *ptr = (char *)MALLOC(size);
  for (int i = 0; i < size; i++) ptr[i] = 0;
  return ptr;
}

// Tree Node
typedef struct Node_struct {
  struct Node_struct *left, *right;
  int i, j;
} Node0;

typedef Node0 *Node;

void init_Node(Node me, Node l, Node r) {
  me->left = l;
  me->right = r;
}

void destroy_Node(Node me) {
  if (me->left) destroy_Node(me->left);
  if (me->right) destroy_Node(me->right);
  FREE(me);
}

int TreeSize(int depth) {
  return (1 << (depth + 1)) - 1;
}

int NumIters(int depth) {
  return 2 * TreeSize(4) / TreeSize(depth);
}

void Populate(int depth, Node node) {
  if (depth <= 0) return;
  depth--;
  node->left = allocate_node(sizeof(Node0));
  node->right = allocate_node(sizeof(Node0));
  Populate(depth, node->left);
  Populate(depth, node->right);
}

Node MakeTree(int depth) {
  if (depth <= 0) {
    return allocate_node(sizeof(Node0));
  } else {
    Node left = MakeTree(depth - 1);
    Node right = MakeTree(depth - 1);
    Node result = allocate_node(sizeof(Node0));
    init_Node(result, left, right);
    return result;
  }
}

void TimeConstruction(int depth) {
  int iters = NumIters(depth);
  print_string("Creating ", 9);
  print_int(iters);
  print_string(" trees of depth ", 16);
  print_int(depth);
  print_string("\n", 2);

  for (int i = 0; i < iters; ++i) {
    Node tmp = allocate_node(sizeof(Node0));
    Populate(depth, tmp);
    destroy_Node(tmp);
  }

  print_string("\tTop down complete\n", 21);

  for (int i = 0; i < iters; ++i) {
    Node tmp = MakeTree(depth);
    destroy_Node(tmp);
  }

  print_string("\tBottom up complete\n", 22);

  print_string("Allocations: ", 13);
  print_int(total_allocs);
  print_string(", Frees: ", 9);
  print_int(total_frees);
  print_string("\n", 2);
}

void start() {
  INIT_MEM;

  print_string("CHERI Binary Trees Benchmark\n", 30);

  Node longLivedTree = allocate_node(sizeof(Node0));
  Populate(6, longLivedTree);

  for (int depth = 4; depth <= 8; depth += 2) {
    TimeConstruction(depth);
  }

  if (!longLivedTree) {
    print_string("Error: longLivedTree is NULL\n", 30);
  }
}
