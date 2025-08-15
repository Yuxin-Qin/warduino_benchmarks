// Simple CHERI-compatible print benchmark
// Tests print_string and print_int primitives

#include <stdint.h>

void print_string(const char *s);
void print_int(int n);

void start() {
  print_string("=== WARDuino CHERI Print Benchmark ===\n", 39);

  print_string("Printing integers:\n", 18);
  for (int i = 0; i <= 5; i++) {
    print_int(i);
  }

  print_string("Printing test strings:\n", 21);
  print_string("Hello, world!\n", 14);
  print_string("CHERI-safe I/O test\n", 20);
  print_string("Print benchmark completed.\n", 26);

  print_string("=== END ===\n", 12);
}
