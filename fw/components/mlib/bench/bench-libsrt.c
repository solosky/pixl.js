#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "svector.h"
#include "smap.h"
#include "smset.h"

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  srt_vector *a1 = sv_alloc ( sizeof (unsigned int), 16, NULL);
  srt_vector *a2 = sv_alloc ( sizeof (unsigned int), 16, NULL);

  for(size_t i = 0; i < n; i++) {
    unsigned int value = rand_get();
    sv_push(&a1, &value );
    value = rand_get();
    sv_push(&a2, &value );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *(unsigned int*)sv_at(a1, i ) * *(unsigned int *)sv_at(a2, i );
  }
  g_result = s;
  
  sv_free(&a1);
  sv_free(&a2);
}

/********************************************************************************************/

/* There is no 'unsigned long' key in smset */

/********************************************************************************************/

static int cmp_float (const void *a, const void *b)
{
  const float *pa=a;
  const float *pb = b;
  return (*pa < *pb) ? -1 : *pa > *pb;
}

static void test_sort(size_t n)
{
  srt_vector *a1 = sv_alloc(sizeof(float), 16, cmp_float);
  for(size_t i = 0; i < n; i++) {
    unsigned int value = rand_get();
    sv_push(&a1, &value );
  }
  sv_sort(a1);
  g_result = *(float*)sv_at(a1, 0);
  sv_free(&a1);
}

/********************************************************************************************/


const config_func_t table[] = {
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 50,    "sort", 10000000, 0, test_sort, 0}
};

int main(int argc, const char *argv[])
{
  test("LIBSRT", numberof(table), table, argc, argv);
  exit(0);
}

