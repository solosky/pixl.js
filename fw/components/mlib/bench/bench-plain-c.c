// Inspired by: http://stackoverflow.com/questions/3664272/is-stdvector-so-much-slower-than-plain-arrays/3664349#3664349

#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "m-array.h"
#include "m-list.h"
#include "m-rbtree.h"
#include "m-dict.h"
#include "m-algo.h"
#include "m-mempool.h"
#include "m-string.h"

unsigned long g_result;

static unsigned long long
cputime (void)
{
  struct rusage rus;
  getrusage (0, &rus);
  return rus.ru_utime.tv_sec * 1000000ULL + rus.ru_utime.tv_usec;
}

struct Pixel
{
  unsigned char r, g, b;
};

ARRAY_DEF(array_pixel, struct Pixel, (INIT(M_NOTHING_DEFAULT), INIT_SET(M_MEMCPY_DEFAULT) ))

void UseMlib(void)
{
  unsigned long long t1 = cputime();

  for(int i = 0; i < 1000; ++i)
    {
      int dimension = 999;
      
      array_pixel_t pixels;
      array_pixel_init(pixels);
      array_pixel_reserve(pixels, dimension * dimension);

      for(int i = 0; i < dimension * dimension; ++i)
        {
          const struct Pixel p = { 255, 0, 0};
          array_pixel_push_back (pixels, p);
        }
      
      array_pixel_clear(pixels);
    }
  unsigned long long t2 = cputime();
  printf ("M*Lib  Array: %Lu us\n", (t2-t1)/1000);
}

void UseNative(void)
{
  unsigned long long t1 = cputime();

  for(int i = 0; i < 1000; ++i)
    {
      int dimension = 999;
      
      struct Pixel * pixels = malloc(sizeof(struct Pixel) * dimension * dimension);
      
      for(int i = 0 ; i < dimension * dimension; ++i)
        {
          pixels[i].r = 255;
          pixels[i].g = 0;
          pixels[i].b = 0;
        }
      
      free(pixels);
    }
  unsigned long long t2 = cputime();
  printf ("Native Array: %Lu us\n", (t2-t1)/1000);
}

int main(void)
{
  UseNative();
  UseMlib();
  
  return 0;
}
