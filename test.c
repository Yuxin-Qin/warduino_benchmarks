// Simple CHERI-compatible print benchmark
// Tests print_string and print_int primitives

#include <stdint.h>

void print_string(const char *s);
void print_int(int n);

void start() {
  print_string("=== WARDuino CHERI Print Benchmark ===\n");

  print_string("Printing integers:\n");
  for (int i = 0; i <= 5; i++) {
    print_int(i);
  }

  print_string("Printing test strings:\n");
  print_string("Hello, world!\n");
  print_string("CHERI-safe I/O test\n");
  print_string("Print benchmark completed.\n");

  print_string("=== END ===");
}