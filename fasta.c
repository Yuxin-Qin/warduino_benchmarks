// CHERI-compatible FASTA benchmark
// Ported from fasta.gcc-2.c
// Custom memory-safe version avoiding malloc, printf, and unsafe pointer arithmetic

#define MAXIMUM_LINE_WIDTH 60
#define LINES_PER_BLOCK 1024
#define CHARACTERS_PER_BLOCK (MAXIMUM_LINE_WIDTH * LINES_PER_BLOCK)

#include <stdint.h>
#include <stddef.h>

// Capability-safe I/O stubs (to be provided externally)
void print_string(const char *s);
void print_char(char c);
void print_buffer(const char *buf, size_t len);
void *my_malloc(unsigned int size);
void my_free(void *ptr);

typedef intptr_t intnative_t;

typedef struct {
  char letter;
  float probability;
} nucleotide_info;

// Repeat and wrap string into output
void repeat_and_wrap_string(const char *str, intnative_t total_chars) {
  intnative_t str_len = 0;
  while (str[str_len] != '\0') str_len++;
  intnative_t line_len = MAXIMUM_LINE_WIDTH;
  intnative_t total_printed = 0;
  intnative_t offset = 0;
  while (total_printed < total_chars) {
    int to_copy = line_len;
    if (total_chars - total_printed < line_len)
      to_copy = total_chars - total_printed;
    for (int i = 0; i < to_copy; i++) {
      print_char(str[(offset + i) % str_len]);
    }
    print_char('\n');
    offset = (offset + to_copy) % str_len;
    total_printed += to_copy;
  }
}

// Pseudo-random generator (LCG)
#define IM 139968
#define IA 3877
#define IC 29573
static uint32_t seed = 42;

uint32_t next_rand() {
  seed = (seed * IA + IC) % IM;
  return seed;
}

// Generate wrapped pseudorandom DNA sequence
void generate_and_wrap_pseudorandom_dna(const nucleotide_info *info, int count, intnative_t total_chars) {
  uint32_t thresholds[count];
  float acc = 0.0;
  for (int i = 0; i < count; i++) {
    acc += info[i].probability;
    thresholds[i] = (uint32_t)(acc * IM);
  }

  char line[MAXIMUM_LINE_WIDTH + 1];
  int col = 0;
  for (intnative_t i = 0; i < total_chars; i++) {
    uint32_t r = next_rand();
    int j;
    for (j = 0; j < count - 1; j++) {
      if (r < thresholds[j]) break;
    }
    line[col++] = info[j].letter;
    if (col == MAXIMUM_LINE_WIDTH) {
      line[col] = '\0';
      print_string(line);
      print_char('\n');
      col = 0;
    }
  }
  if (col > 0) {
    line[col] = '\0';
    print_string(line);
    print_char('\n');
  }
}

void start(int n) {
  print_string(">ONE Homo sapiens alu\n");
  const char alu[] =
    "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTC"
    "AGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCG"
    "TGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGG"
    "AGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";
  repeat_and_wrap_string(alu, 2 * n);

  print_string(">TWO IUB ambiguity codes\n");
  nucleotide_info iub[] = {
    { 'a', 0.27 },{ 'c', 0.12 },{ 'g', 0.12 },{ 't', 0.27 },{ 'B', 0.02 },
    { 'D', 0.02 },{ 'H', 0.02 },{ 'K', 0.02 },{ 'M', 0.02 },{ 'N', 0.02 },
    { 'R', 0.02 },{ 'S', 0.02 },{ 'V', 0.02 },{ 'W', 0.02 },{ 'Y', 0.02 }
  };
  generate_and_wrap_pseudorandom_dna(iub, sizeof(iub)/sizeof(nucleotide_info), 3 * n);

  print_string(">THREE Homo sapiens frequency\n");
  nucleotide_info homos[] = {
    { 'a', 0.3029549426680 },{ 'c', 0.1979883004921 },
    { 'g', 0.1975473066391 },{ 't', 0.3015094502008 }
  };
  generate_and_wrap_pseudorandom_dna(homos, sizeof(homos)/sizeof(nucleotide_info), 5 * n);
}
