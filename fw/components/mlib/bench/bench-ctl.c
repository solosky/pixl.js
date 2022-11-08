#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "m-core.h"
#include "common.h"

typedef unsigned int uint;
typedef unsigned long ulong;

/********************************************************************************************/
#define POD
#define T uint
#include "ctl/vector.h"

static void test_array(size_t n)
{
  vec_uint a1 = vec_uint_init();
  vec_uint a2 = vec_uint_init();
  
  for(size_t i = 0; i < n; i++) {
    vec_uint_push_back(&a1, rand_get() );
    vec_uint_push_back(&a2, rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *vec_uint_at(&a1, i ) * *vec_uint_at(&a2, i );
  }
  g_result = s;

  vec_uint_free(&a1);
  vec_uint_free(&a2);
}

/********************************************************************************************/

#define POD
#define T uint
#include "ctl/forward_list.h"

static void test_list (size_t n)
{
  slist_uint a1 = slist_uint_init();
  slist_uint a2 = slist_uint_init();

  for(size_t i = 0; i < n; i++) {
    slist_uint_push_front(&a1, rand_get());
    slist_uint_push_front(&a2, rand_get());
  }
  unsigned int s = 0;
  slist_uint_it it1 = slist_uint_begin(&a1);
  slist_uint_it it2 = slist_uint_begin(&a2);
  for( ; !slist_uint_it_done(&it1) ;
       slist_uint_it_next(&it1), slist_uint_it_next(&it2)) {
    s += *slist_uint_it_ref(&it1) * *slist_uint_it_ref(&it2);
  }
  g_result = s;

  slist_uint_free(&a1);
  slist_uint_free(&a2);
}

/********************************************************************************************/

#define POD
#define T ulong
#include "ctl/set.h"

static inline int
int_cmp(ulong *a, ulong *b) {
  return *a < *b;
}

static void test_rbtree(size_t n)
{
  set_ulong tree = set_ulong_init (int_cmp);
  
  for (size_t i = 0; i < n; i++) {
    set_ulong_insert (&tree, rand_get() );
  }
    
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    if (set_ulong_find_node(&tree, j) != NULL)
      s+= j;
  }
  g_result = s;
  set_ulong_free(&tree);
}

/********************************************************************************************/

typedef struct {
  unsigned long key;
  unsigned long value;
} pair_ulong;

// Use of M*LIB hash function
static inline size_t
pair_ulong_hash(pair_ulong *a)
{
  return M_HASH_DEFAULT(a->key);
}

static inline int
pair_ulong_equal(pair_ulong *a, pair_ulong *b)
{
  return a->key == b->key;
}

static inline void
pair_ulong_free(pair_ulong *a) {
  (void) a;
}

static inline pair_ulong
pair_ulong_copy(pair_ulong *self) {
  return (pair_ulong) { .key = self->key, .value = self->value };
}

#define T pair_ulong
#include <ctl/unordered_map.h>

static void
test_dict(size_t  n)
{
  umap_pair_ulong dict = umap_pair_ulong_init(pair_ulong_hash, pair_ulong_equal);
  
  for (size_t i = 0; i < n; i++) {
    unsigned long value = rand_get();
    unsigned long key = rand_get();
    umap_pair_ulong_insert(&dict, (pair_ulong) { key, value });
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    umap_pair_ulong_it it = umap_pair_ulong_find(&dict, (pair_ulong) { rand_get() , 0 } );
    if (!umap_pair_ulong_it_done(&it) )
      s += it.ref->value;
  }
  g_result = s;

  umap_pair_ulong_free(&dict);
}

/********************************************************************************************/

#define POD
#define T float
#include "ctl/vector.h"

static void test_sort(size_t n)
{
  vec_float a1 = vec_float_init();

  for(size_t i = 0; i < n; i++) {
    vec_float_push_back(&a1, rand_get());
  }
  vec_float_sort(&a1);
  g_result = *vec_float_at(&a1, 0);
  vec_float_free(&a1);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List",  10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 30,  "Rbtree",   1000000, 0, test_rbtree, 0},
  { 40,    "dict",   1000000, 0, test_dict, 0},
  { 50,    "Sort",  10000000, 0, test_sort, 0},
};

int main(int argc, const char *argv[])
{
  test("CTL", numberof(table), table, argc, argv);
  exit(0);
}

