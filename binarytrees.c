// adapted from Hans Boehm's GC benchmark
// by Jeremy.Singer@glasgow.ac.uk
//   -> See https://www.hboehm.info/gc/gc_bench.html
// This code is equivalent to "binarytrees" from
// the Programming Language Shootout benchmark game
//   -> See http://benchmarksgame.alioth.debian.org/u64q/performance.php?test=binarytrees

// Adapted for WARDuino execution - see README.md

#define MALLOC   my_malloc
#define FREE     my_free
#define INIT_MEM init_mem_pool()
#define NULL 0

/***********
 * MEMORY ALLOCATOR IMPLEMENTATION
 * This uses a dedicated memory pool
 * with an integrated free-list
 * so the allocator becomes a simple
 * free-list allocation algorithm
 */

#define CELL_SIZE 32

typedef union {
  char bytes[CELL_SIZE];
  void *ptr;
} Cell;

#define POOL_SIZE_IN_PAGES 2000 /* 2 Kpages == 8 MB */
#define PAGE_SIZE 1<<12         /* 4KB per page */

char mem[POOL_SIZE_IN_PAGES * PAGE_SIZE];

void *pool = NULL;
Cell *freelist = NULL;

void init_mem_pool() {
  
  void *p = &mem[0];
  unsigned int pool_size = POOL_SIZE_IN_PAGES * PAGE_SIZE;
  Cell *cell = (Cell *)p;
  while (cell < (p+pool_size)-32) {
    Cell *next = cell+1;;
    cell->ptr = next;
    cell = next;
  }
  cell->ptr = NULL; // the tail of the list
  // freelist points to start of this ordered
  // linked list sequence
  freelist = (Cell *)p;
  pool = p;
  return;
}

/*
 * malloc routine:
 * This routine allocates one of the fixed size blocks ---
 * we only have the num_bytes argument for compatibility with
 * the standard malloc routine. You might do an assertion
 * to check the size is the same as your fixed size cells.
 * Your allocation routine needs to return a pointer to
 * one of these cells, and update the metadata so the returned
 * cell is no longer on the freelist.
 * NOTE: if no memory is available then return NULL
 */
void *my_malloc(unsigned int num_bytes) {
  void *p;
  if (freelist == NULL) {
    p = NULL;
  }
  else {
    p = (void *)freelist;
    freelist = freelist->ptr;
  }
  return p;
}

/**
 * free routine
 * Given a pointer to a fixed size cell, you need to
 * add this cell to the freelist, updating metadata
 * as appropriate.
 */
void my_free(void *ptr) {
  Cell *empty = (Cell *)ptr;
  // push onto front of freelist
  empty->ptr = freelist;
  freelist = empty;
  return;
}





// allocate a single tree node of @param size
// and zero out all data bytes
// like calloc, or Java allocation
void *allocate_node(int size) {
  char *ptr = (char *)MALLOC(size);
  int i;
  for (i=0; i<size; i++) {
    ptr[i] = 0;
  }
  return ptr;
}

static const int kStretchTreeDepth    = 4; //= 18;      // about 16Mb
  static const int kLongLivedTreeDepth  =6; //= 16;  // about 4Mb
static const int kArraySize  =100; //= 500000;  // about 4Mb
static const int kMinTreeDepth = 4;
static const int kMaxTreeDepth = 8; // = 16;

// on x86_64, this structure has size 24 bytes
typedef struct Node0_struct {
        struct Node0_struct * left;
        struct Node0_struct * right;
        int i, j;
} Node0;


typedef Node0 *Node;

void init_Node(Node me, Node l, Node r) {
    me -> left = l;
    me -> right = r;
}

void destroy_Node(Node me) {
  if (me -> left) {
    destroy_Node(me -> left);
  }
  if (me -> right) {
    destroy_Node(me -> right);
  }
  FREE(me);
}


// Nodes used by a tree of a given size
static int TreeSize(int i) {
        return ((1 << (i + 1)) - 1);
}

// Number of iterations to use for a given tree depth
static int NumIters(int i) {
        return 2 * TreeSize(kStretchTreeDepth) / TreeSize(i);
}

// Build tree top down, assigning to older objects.
static void Populate(int iDepth, Node thisNode) {
        if (iDepth<=0) {
                return;
        } else {
	        iDepth--;
		thisNode->left  = allocate_node(sizeof(Node0));
		thisNode->right = allocate_node(sizeof(Node0));
                Populate (iDepth, thisNode->left);
                Populate (iDepth, thisNode->right);
        }
}

// Build tree bottom-up
static Node MakeTree(int iDepth) {
	Node result;
        if (iDepth<=0) {
	  result = allocate_node(sizeof(Node0));
	  /* result is implicitly initialized in both cases. */
	  return result;
        } else {
	    Node left = MakeTree(iDepth-1);
	    Node right = MakeTree(iDepth-1);
	    result = allocate_node(sizeof(Node0));
	    init_Node(result, left, right);
	    return result;
        }
}


static void TimeConstruction(int depth) {
        long    tStart, tFinish;
        int     iNumIters = NumIters(depth);
        Node    tempTree;
	int 	i;

	print_string("Creating ");
	print_int(iNumIters);
	print_string(" trees of depth ");
	print_int(depth);
	print_string("\n");
        
        //tStart = currentTime();
        for (i = 0; i < iNumIters; ++i) {
	  tempTree = allocate_node(sizeof(Node0));
	  Populate(depth, tempTree);
	  destroy_Node(tempTree);
	  tempTree = 0;
        }
        //tFinish = currentTime();
        print_string("\tTop down construction complete\n");
             
        //tStart = currentTime();
        for (i = 0; i < iNumIters; ++i) {
                tempTree = MakeTree(depth);
		destroy_Node(tempTree);
                tempTree = 0;
        }
        //tFinish = currentTime();
        print_string("\tBottom up construction complete\n");

}

// benchmark entrypoint
void start() {
        Node    root;
        Node    longLivedTree;
        Node    tempTree;
        long    tStart, tFinish;
        long    tElapsed;
  	int	i, d;
	double 	*array;

	print_int(1);
	INIT_MEM;
	print_int(2);
	
	print_string("Memory Allocator Test\n");
 	print_string(" Live storage will peak at ");
	print_int(2 * sizeof(Node0) * TreeSize(kLongLivedTreeDepth) +
		  sizeof(double) * kArraySize);
	print_string(" bytes.\n\n");
        print_string(" Stretching memory with a binary tree of depth ");
	print_int(kStretchTreeDepth);
	print_string("\n");
        //PrintDiagnostics();
       
        //tStart = currentTime();
        
        // Stretch the memory space quickly
        tempTree = MakeTree(kStretchTreeDepth);
	destroy_Node(tempTree);
        tempTree = 0;

        // Create a long lived object
        print_string(" Creating a long-lived binary tree of depth ");
	print_int(kLongLivedTreeDepth);
	print_string("\n");
	longLivedTree = allocate_node(sizeof(Node0));
        Populate(kLongLivedTreeDepth, longLivedTree);

        //PrintDiagnostics();

        for (d = kMinTreeDepth; d <= kMaxTreeDepth; d += 2) {
                TimeConstruction(d);
        }

        if (longLivedTree == 0)
	  print_string("Failed\n");
                                // fake reference to LongLivedTree
	                        // to keep them from being optimized away

        //tFinish = currentTime();
        //tElapsed = elapsedTime(tFinish-tStart);
        //PrintDiagnostics();
        //printf("Completed in %ld msec\n", tElapsed);
}
