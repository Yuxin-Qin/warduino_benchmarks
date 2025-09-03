#define MALLOC my_malloc  
#define FREE my_free
#define INIT_MEM init_mem_pool()
#define NULL 0

#include <stdint.h>
#define CELL_SIZE 32
#define POOL_SIZE_IN_PAGES 2000  
#define PAGE_SIZE (1<<12)

char mem[POOL_SIZE_IN_PAGES*PAGE_SIZE]; 
void* pool = NULL;

typedef union{
    char bytes[CELL_SIZE];
    void *ptr;
}Cell;

Cell *freelist = NULL;
int total_allocs = 0;
int total_frees = 0;

void print_string(char *str);
void print_int(int n);
// void print_char_alt(char c);
void print_char_alt(char c) {
    char temp[2];
    temp[0] = c;
    temp[1] = '\0';
    print_string(temp);
}


void init_mem_pool(){
    void *p = &mem[0];
    unsigned int pool_size = POOL_SIZE_IN_PAGES * PAGE_SIZE; 
    Cell *cell = (Cell *)p; 
    while((uintptr_t)cell < (uintptr_t)p + pool_size-CELL_SIZE){
        Cell *next = cell + 1;
        cell->ptr = next;
        cell = next;
    }
    cell->ptr =NULL;
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


static char complement_lookup[256];

void init_complement_lookup() {
  
  for (int i = 0; i < 256; i++) {
    complement_lookup[i] = (char)i;
  }
  
  
  complement_lookup['A'] = 'T'; complement_lookup['T'] = 'A';
  complement_lookup['C'] = 'G'; complement_lookup['G'] = 'C';
  complement_lookup['a'] = 't'; complement_lookup['t'] = 'a';
  complement_lookup['c'] = 'g'; complement_lookup['g'] = 'c';
  
  
  complement_lookup['Y'] = 'R'; complement_lookup['R'] = 'Y';
  complement_lookup['S'] = 'S'; complement_lookup['W'] = 'W';
  complement_lookup['K'] = 'M'; complement_lookup['M'] = 'K';
  complement_lookup['B'] = 'V'; complement_lookup['V'] = 'B';
  complement_lookup['D'] = 'H'; complement_lookup['H'] = 'D';
  complement_lookup['N'] = 'N';
  complement_lookup['y'] = 'r'; complement_lookup['r'] = 'y';
  complement_lookup['s'] = 's'; complement_lookup['w'] = 'w';
  complement_lookup['k'] = 'm'; complement_lookup['m'] = 'k';
  complement_lookup['b'] = 'v'; complement_lookup['v'] = 'b';
  complement_lookup['d'] = 'h'; complement_lookup['h'] = 'd';
  complement_lookup['n'] = 'n';
}


// String utility functions
int my_strlen(const char *str)
{
    int len = 0;
    while(str[len]) len++;
    return len;
}

void my_strcpy(char *dest, const char *src)
{
    while((*dest++ = *src++));
}

char *my_strstr(const char *str,int c)
{
    while(*str){
        if(*str == c)return (char *)str;
        str++;
    }
    return NULL;
}


void process_sequence(char *sequence, int sequence_size) {
  if (!sequence || sequence_size <= 0) return;
  
  // Find the start of actual sequence data (skip header line)
  char *data_start = sequence;
  while (*data_start && *data_start != '\n') data_start++;
  if (*data_start == '\n') data_start++;
  
  // Calculate actual data length (excluding newlines)
  int data_len = 0;
  char *temp = data_start;
  while (temp < sequence + sequence_size) {
    if (*temp != '\n') data_len++;
    temp++;
  }
  
  // Create a clean sequence without newlines
  char *clean_seq = (char *)MALLOC(data_len + 1);
  if (!clean_seq) return;
  
  int clean_idx = 0;
  temp = data_start;
  while (temp < sequence + sequence_size) {
    if (*temp != '\n') {
      clean_seq[clean_idx++] = *temp;
    }
    temp++;
  }
  clean_seq[clean_idx] = '\0';
  
  // Reverse and complement the sequence
  for (int i = 0; i < data_len / 2; i++) {
    char front = clean_seq[i];
    char back = clean_seq[data_len - 1 - i];
    
    clean_seq[i] = complement_lookup[(unsigned char)back];
    clean_seq[data_len - 1 - i] = complement_lookup[(unsigned char)front];
  }
  
  // Handle middle character for odd-length sequences
  if (data_len % 2 == 1) {
    int mid = data_len / 2;
    clean_seq[mid] = complement_lookup[(unsigned char)clean_seq[mid]];
  }
  
  // Output the header (copy from original)
  char *header_end = data_start - 1;
  while (sequence < header_end) {
    print_int((int)*sequence++);
  }
  print_char_alt('\n');
  
  // Output the processed sequence with line breaks every 60 characters
  for (int i = 0; i < data_len; i++) {
    if (i > 0 && i % 60 == 0) {
      print_char_alt('\n');
    }
    print_char_alt(clean_seq[i]);
  }
  print_char_alt('\n');
  
  FREE(clean_seq);
}


void start() {
  INIT_MEM;
  
  print_string("WARDuino Reverse Complement Benchmark\n");
  
 
  const char *test_sequences[] = {
    ">seq1 test sequence\nATCGGCTA\nGCTAATCG\n",
    ">seq2 short\nAAAATTTT\nCCCCGGGG\n",
    ">seq3 tiny\nATGC\n"
  };
  
  int num_sequences = 3;
  
  for (int i = 0; i < num_sequences; i++) {
    int seq_len = my_strlen(test_sequences[i]);
    char *sequence = (char *)MALLOC(seq_len + 1);
    
    if (sequence) {
      my_strcpy(sequence, test_sequences[i]);
      
      print_string("\nOriginal sequence ");
      print_int(i + 1);
      print_string(":\n");
      
     
      for (int j = 0; j < seq_len; j++) {
        print_char_alt(sequence[j]);
      }
      
      
      process_sequence(sequence, seq_len);
      
      print_string("\n--- End of sequence ");
      print_int(i + 1);
      print_string(" ---\n");
      
      FREE(sequence);
    } else {
      print_string("Memory allocation failed\n");
    }
  }
  
  print_string("\nMemory stats - Allocs: ");
  print_int(total_allocs);
  print_string(", Frees: ");
  print_int(total_frees);
  print_string("\nBenchmark completed\n");
}
