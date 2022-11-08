#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"

/********************************************************************************************/

#define i_val unsigned int
#define i_tag uint
#include <stc/cvec.h>

static void test_array(size_t n)
{
  cvec_uint a1 = cvec_uint_init(), a2 = cvec_uint_init();
  
  for(size_t i = 0; i < n; i++) {
    cvec_uint_push_back(&a1, rand_get() );
    cvec_uint_push_back(&a2, rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *cvec_uint_at(&a1, i ) * *cvec_uint_at(&a2, i );
  }
  g_result = s;

  cvec_uint_drop(&a1);
  cvec_uint_drop(&a2);
}

/********************************************************************************************/
#define i_val unsigned int
#define i_tag uint
#include <stc/clist.h>

static void test_list(size_t n)
{
  clist_uint a1 = clist_uint_init(), a2 = clist_uint_init();
  
  for(size_t i = 0; i < n; i++) {
    clist_uint_push_front(&a1, rand_get() );
    clist_uint_push_front(&a2, rand_get() );
  }
  unsigned int s = 0;
  for(clist_uint_iter it1 = clist_uint_begin(&a1), it2 = clist_uint_begin(&a2) ;
      it1.ref != clist_uint_end(&a1).ref ;
      clist_uint_next(&it1), clist_uint_next(&it2) ) {
    s += *it1.ref * *it2.ref;
  }
  g_result = s;

  clist_uint_drop(&a1);
  clist_uint_drop(&a2);
}

/********************************************************************************************/

#define i_tag ulong
#define i_key unsigned long
#include <stc/csset.h>

static void test_rbtree(size_t n)
{
  csset_ulong tree = csset_ulong_init();
  
  for (size_t i = 0; i < n; i++) {
    csset_ulong_insert(&tree, rand_get() );
  }
    
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    if (csset_ulong_get(&tree, j ) != NULL)
      s+= j;
  }
  g_result = s;
  csset_ulong_drop(&tree);
}

/********************************************************************************************/

#define i_tag ulong
#define i_key unsigned long
#define i_val unsigned long
#include <stc/cmap.h>

static void
test_dict(size_t  n)
{
  cmap_ulong dict = cmap_ulong_init();
  
  for (size_t i = 0; i < n; i++) {
    unsigned long value = rand_get();
    unsigned long key = rand_get();
    cmap_ulong_put(&dict, key, value);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    const cmap_ulong_value*val = cmap_ulong_get(&dict, rand_get());
    if (val != NULL)
      s += val->second;
  }
  g_result = s;
  
  cmap_ulong_drop(&dict);
}

/********************************************************************************************/

typedef struct {
  char a[256];
} char_array_t;
static inline void char_init (char_array_t *a) { a->a[0] = 0; }
static inline void char_set (char_array_t *a, const char_array_t b) { strcpy(a->a, b.a); }
static inline bool char_equal_p (const char_array_t *a, const char_array_t *b) { return strcmp(a->a,b->a)==0; }
static inline void char_clear (char_array_t *a) { (void)a; }
static inline size_t char_hash(const char_array_t *a, size_t size) {
  (void) size;
  size_t hash = 0;
  const char *s = a->a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

#define i_tag       achar
#define i_key       char_array_t
#define i_val       char_array_t
#define i_hash      char_hash
#define i_eq        char_equal_p
#include <stc/cmap.h>

static void
test_dict_big(size_t  n)
{
  cmap_achar dict = cmap_achar_init();

  for (size_t i = 0; i < n; i++) {
    char_array_t s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    cmap_achar_put(&dict, s1, s2);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1.a, "%u", rand_get());
    const cmap_achar_value*val = cmap_achar_get(&dict, s1);
    if (val != NULL)
      s ++;
  }
  g_result = s;

  cmap_achar_drop(&dict);
}

/********************************************************************************************/

#define i_val float
#define i_tag float
#include <stc/cvec.h>

static void test_sort(size_t n)
{
  cvec_float a1 = cvec_float_init();

  for(size_t i = 0; i < n; i++) {
    cvec_float_push_back(&a1, rand_get() );
  }
  cvec_float_sort(&a1);
  g_result = *cvec_float_at(&a1, 0);
  cvec_float_drop(&a1);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
  { 50,    "Sort",10000000, 0, test_sort, 0},
};

int main(int argc, const char *argv[])
{
  test("STC", numberof(table), table, argc, argv);
  exit(0);
}
