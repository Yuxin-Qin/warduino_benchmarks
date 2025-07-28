// Simple CHERI-compatible print benchmark
// Tests print_string and print_int primitives

#include <stdint.h>

void print_string(const char *s);
void print_int(int n);

void start() {
  print_string("=== WARDuino CHERI Print Benchmark ===");
  print_string("\n");

  print_string("Printing integers:");
  print_string("\n");
  for (int i = 0; i <= 5; i++) {
    print_int(i);
  }

  print_string("Printing test strings:");
  print_string("\n");
  print_string("Hello, world!");
  print_string("\n");
  print_string("CHERI-safe I/O test");
  print_string("\n");
  print_string("Print benchmark completed.");
  print_string("\n");

  print_string("=== END ===");
  print_string("\n");
}