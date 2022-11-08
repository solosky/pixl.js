#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "macro_collections.h"

#include "common.h"

/********************************************************************************************/

// CMC list are dynamic array
#define MY_LIST_PARAMS (list_uint, list_uint_s, , , unsigned int)
C_MACRO_COLLECTIONS_EXTENDED(CMC, LIST, MY_LIST_PARAMS, (STR))

struct list_uint_s_fval list_fval = { .cmp = NULL,
				      .cpy = NULL,
				      .str = NULL,
				      .free = NULL,
				      .hash = NULL,
				      .pri = NULL };

static void test_array(size_t n)
{
  struct list_uint_s *a1 = list_uint_new(1, &list_fval);
  struct list_uint_s *a2 = list_uint_new(1, &list_fval);
  
  for(size_t i = 0; i < n; i++) {
    list_uint_push_back(a1, rand_get() );
    list_uint_push_back(a2, rand_get() );
  }
  
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += list_uint_get(a1, i ) * list_uint_get(a2, i );
  }
  g_result = s;
  
  list_uint_free(a1);
  list_uint_free(a2);
}

/********************************************************************************************/

#define my_cmp(a,b) ( (a) < (b) ? -1 : (a) > (b) )

#define MY_TMAP_PARAMS (tree_ulong, tree_ulong_s, , unsigned long, bool)
C_MACRO_COLLECTIONS_EXTENDED(CMC, TREEMAP, MY_TMAP_PARAMS, (STR))

int cmp_ulong(unsigned long a, unsigned long b) { return my_cmp(a,b); }

struct tree_ulong_s_fkey tree_fkey = { .cmp = cmp_ulong,
				       .cpy = NULL,
				       .str = NULL,
				       .free = NULL,
				       .hash = NULL,
				       .pri = NULL };

struct tree_ulong_s_fval tree_fval = { .cmp = NULL,
				       .cpy = NULL,
				       .str = NULL,
				       .free = NULL,
				       .hash = NULL,
				       .pri = NULL };

static void test_rbtree(size_t n)
{
  struct tree_ulong_s *tree;
  tree = tree_ulong_new(&tree_fkey, &tree_fval);

  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    tree_ulong_insert(tree, j, true);
  }
    
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    if (tree_ulong_get_ref(tree, j) != 0)
      s+= j;
  }
  g_result = s;
  tree_ulong_free(tree);
}

/********************************************************************************************/
#define hash_func(a) kh_int64_hash_func(a)
size_t hash_ulong(unsigned long a)
{
  return ( (a) >> 33 ) ^ (a) ^ ((a) << 11);
}

#define MY_DICT_PARAMS (hmap, hmap_s, , unsigned long, unsigned long)
C_MACRO_COLLECTIONS_EXTENDED(CMC, HASHMAP, MY_DICT_PARAMS, (STR))

struct hmap_s_fkey hmap_fkey = { .cmp = cmp_ulong,
				 .cpy = NULL,
				 .str = NULL,
				 .free = NULL,
				 .hash = hash_ulong,
				 .pri = NULL };

struct hmap_s_fval hmap_fval = { .cmp = NULL,
				 .cpy = NULL,
				 .str = NULL,
				 .free = NULL,
				 .hash = NULL,
				 .pri = NULL };

static void
test_dict(size_t  n)
{
  struct hmap_s *dict = hmap_new(16, 0.7, &hmap_fkey, &hmap_fval);
  
  for (size_t i = 0; i < n; i++) {
    hmap_insert(dict, rand_get(), rand_get());
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long *val = hmap_get_ref(dict, rand_get());
    if (val != 0)
      s += *val;
  }
  g_result = s;
  
  hmap_free(dict);
}

/********************************************************************************************/

#if 0 // Not working issue in using not integer types

typedef struct {
  char a[256];
} char_array_t;
static inline int char_cmp (const char_array_t a, const char_array_t b) { return strcmp(a.a,b.a); }
static inline size_t char_hash(const char_array_t a) {
  size_t hash = 0;
  const char *s = a.a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

#define MY_DICTB_PARAMS (bhmap, bhmap_s, , char_array_t, char_array_t)
C_MACRO_COLLECTIONS_EXTENDED(CMC, HASHMAP, MY_DICT_PARAMS, (STR))

struct bhmap_s_fkey bhmap_fkey = { .cmp = char_cmp,
				   .cpy = NULL,
				   .str = NULL,
				   .free = NULL,
				   .hash = char_hash,
				   .pri = NULL };

struct bhmap_s_fval bhmap_fval = { .cmp = NULL,
				 .cpy = NULL,
				 .str = NULL,
				 .free = NULL,
				 .hash = NULL,
				 .pri = NULL };

static void
test_dict_big(size_t  n)
{
  struct bhmap_s *dict = bhmap_new(16, 0.7, &bhmap_fkey, &bhmap_fval);
  
  for (size_t i = 0; i < n; i++) {
    char_array_t s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    bhmap_insert(dict, s1, s2);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1.a, "%u", rand_get() );
    char_array_t *val = bhmap_get_ref(dict, s1);
    if (val != 0)
      s ++;
  }
  g_result = s;
  
  bhmap_free(dict);
}
#endif

/********************************************************************************************/

const config_func_t table[] = {
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  //{ 41, "dictBig", 1000000, 0, test_dict_big, 0},
};

int main(int argc, const char *argv[])
{
  test("CMC", numberof(table), table, argc, argv);
  exit(0);
}

