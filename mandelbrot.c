// The Computer Language Benchmarks Game
// https://salsa.debian.org/benchmarksgame-team/benchmarksgame/
//
// Contributed by Jeremy Zerfas

// This is the square of the limit that pixels will need to exceed in order to
// escape from the Mandelbrot set.
#define LIMIT_SQUARED      4.0
// This controls the maximum amount of iterations that are done for each pixel.
#define MAXIMUM_ITERATIONS   50

//#include <stdint.h>
//#include <stdlib.h>
//#include <stdio.h>

// intptr_t should be the native integer type on most sane systems.
//typedef intptr_t intnative_t;


/***********
 * MEMORY ALLOCATOR IMPLEMENTATION
 * This uses a dedicated memory pool
 * with an integrated free-list
 * so the allocator becomes a simple
 * free-list allocation algorithm
 */

#define CELL_SIZE 1024

typedef union {
  char bytes[CELL_SIZE];
  void *ptr;
} Cell;

#define POOL_SIZE_IN_PAGES 20 /* 2 Kpages == 8 MB */
#define PAGE_SIZE 1<<12         /* 4KB per page */
#define NULL 0

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


// @entrypoint
int start() {
   // Ensure image_Width_And_Height are multiples of 8.
  const int image_Width_And_Height=(32);

   // The image will be black and white with one bit for each pixel. Bits with
   // a value of zero are white pixels which are the ones that "escape" from
   // the Mandelbrot set. We'll be working on one line at a time and each line
   // will be made up of pixel groups that are eight pixels in size so each
   // pixel group will be one byte. This allows for some more optimizations to
   // be done.

  init_mem_pool();
  
   unsigned char * const pixels=my_malloc(image_Width_And_Height*
     image_Width_And_Height/8);

   // Precompute the initial real and imaginary values for each x and y
   // coordinate in the image.
   double initial_r[image_Width_And_Height], initial_i[image_Width_And_Height];
   //#pragma omp parallel for
   for(int xy=0; xy<image_Width_And_Height; xy++){
      initial_r[xy]=2.0/image_Width_And_Height*xy - 1.5;
      initial_i[xy]=2.0/image_Width_And_Height*xy - 1.0;
   }

   //#pragma omp parallel for schedule(guided)
   for(int y=0; y<image_Width_And_Height; y++){
      const double prefetched_Initial_i=initial_i[y];
      for(int x_Major=0; x_Major<image_Width_And_Height; x_Major+=8){

         // pixel_Group_r and pixel_Group_i will store real and imaginary
         // values for each pixel in the current pixel group as we perform
         // iterations. Set their initial values here.
         double pixel_Group_r[8], pixel_Group_i[8];
         for(int x_Minor=0; x_Minor<8; x_Minor++){
            pixel_Group_r[x_Minor]=initial_r[x_Major+x_Minor];
            pixel_Group_i[x_Minor]=prefetched_Initial_i;
         }

         // Assume all pixels are in the Mandelbrot set initially.
         unsigned char eight_Pixels=0xff;

         int iteration=MAXIMUM_ITERATIONS;
         do{
            unsigned char current_Pixel_Bitmask=0x80;
            for(int x_Minor=0; x_Minor<8; x_Minor++){
               const double r=pixel_Group_r[x_Minor];
               const double i=pixel_Group_i[x_Minor];

               pixel_Group_r[x_Minor]=r*r - i*i +
                 initial_r[x_Major+x_Minor];
               pixel_Group_i[x_Minor]=2.0*r*i + prefetched_Initial_i;

               // Clear the bit for the pixel if it escapes from the
               // Mandelbrot set.
               if(r*r + i*i>LIMIT_SQUARED)
                  eight_Pixels&=~current_Pixel_Bitmask;

               current_Pixel_Bitmask>>=1;
            }
         }while(eight_Pixels && --iteration);

         pixels[y*image_Width_And_Height/8 + x_Major/8]=eight_Pixels;
      }
   }

   print_string("completed\n");
   print_int(pixels[0]);
   // Output the image to stdout.
   //fprintf(stdout, "P4\n%jd %jd\n", (intmax_t)image_Width_And_Height,
   //  (intmax_t)image_Width_And_Height);
   //fwrite(pixels, image_Width_And_Height*image_Width_And_Height/8, 1, stdout);

   my_free(pixels);

   return 0;
}
