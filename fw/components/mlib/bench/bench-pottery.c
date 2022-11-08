#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"

/********************************************************************************************/

#define POTTERY_VECTOR_PREFIX vec_uint
#define POTTERY_VECTOR_VALUE_TYPE unsigned int
#define POTTERY_VECTOR_LIFECYCLE_BY_VALUE 1
#include "pottery/vector/pottery_vector_static.t.h"

static void test_array(size_t n)
{
  vec_uint_t a1, a2;

  vec_uint_init(&a1);
  vec_uint_init(&a2);
  
  for(size_t i = 0; i < n; i++) {
    vec_uint_insert_last(&a1, rand_get() );
    vec_uint_insert_last(&a2, rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *vec_uint_at(&a1, i ) * *vec_uint_at(&a2, i );
  }
  g_result = s;

  vec_uint_destroy(&a1);
  vec_uint_destroy(&a2);
}

/********************************************************************************************/

typedef struct foo_s {
  struct foo_s* next;
  struct foo_s* previous;
  int data;
} foo_t;

#define POTTERY_DOUBLY_LINKED_LIST_PREFIX foo_list
#define POTTERY_DOUBLY_LINKED_LIST_VALUE_TYPE foo_t
#include "pottery/doubly_linked_list/pottery_doubly_linked_list_static.t.h"

static void test_list(size_t n)
{
  foo_list_t a1, a2;
  foo_list_init(&a1);
  foo_list_init(&a2);
  for(size_t i = 0; i < n; i++) {
    foo_t *tmp = malloc(sizeof (foo_t));
    if (tmp == NULL) abort();
    tmp->data = rand_get();
    foo_list_link_last(&a1, tmp);

    tmp = malloc(sizeof (foo_t));
    if (tmp == NULL) abort();
    tmp->data = rand_get();
    foo_list_link_last(&a2, tmp);
  }
  unsigned int s = 0;
  for(foo_t *it1 = foo_list_first(&a1), *it2 = foo_list_first(&a2) ;
      foo_list_entry_exists(&a1, it1) ;
      it1 = foo_list_next(&a1, it1), it2 = foo_list_next(&a2, it2)) {
    s += it1->data * it2->data;
  }
  g_result = s;

  for(foo_t *it1 = foo_list_first(&a1);
      foo_list_entry_exists(&a1, it1) ;
      ) {
    foo_t *tmp = it1;
    it1 = foo_list_next(&a1, it1);
    free(tmp);
  }

  for(foo_t *it1 = foo_list_first(&a2);
      foo_list_entry_exists(&a2, it1) ;
      ) {
    foo_t *tmp = it1;
    it1 = foo_list_next(&a2, it1);
    free(tmp);
  }

}

/********************************************************************************************/

// ERROR: it returns the wrong result.
#define POTTERY_TREE_MAP_PREFIX set_ulong
#define POTTERY_TREE_MAP_VALUE_TYPE unsigned long
#define POTTERY_TREE_MAP_LIFECYCLE_MOVE_BY_VALUE 1
#define POTTERY_TREE_MAP_COMPARE_THREE_WAY(a,b) ((a) < (b) ? -1 : (a) > (b))
#define POTTERY_TREE_MAP_LIFECYCLE_DESTROY(a) (void) a
#include "pottery/tree_map/pottery_tree_map_static.t.h"

static void test_rbtree(size_t n)
{
  set_ulong_t tree;
  set_ulong_init(&tree);
  
  for (size_t i = 0; i < n; i++) {
    set_ulong_insert(&tree, rand_get() );
  }
    
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    if (set_ulong_find(&tree, &j ) != NULL)
      s += j;
  }
  g_result = s;
  set_ulong_destroy(&tree);
}


/********************************************************************************************/

#include "m-core.h"  // For M_HASH_DEFAULT
typedef struct bar_s {
  unsigned long key;
  unsigned long value;
} bar_t;

#define POTTERY_OPEN_HASH_MAP_PREFIX map_ulong

#define POTTERY_OPEN_HASH_MAP_VALUE_TYPE bar_t
#define POTTERY_OPEN_HASH_MAP_KEY_TYPE unsigned long
#define POTTERY_OPEN_HASH_MAP_REF_KEY(v) v->key
#define POTTERY_OPEN_HASH_MAP_KEY_HASH(x) M_HASH_DEFAULT(x)
#define POTTERY_OPEN_HASH_MAP_KEY_EQUAL(x, y) x == y
#define POTTERY_OPEN_HASH_MAP_LIFECYCLE_MOVE(p,q) memcpy(p, q, sizeof (bar_t))

#include "pottery/open_hash_map/pottery_open_hash_map_static.t.h"

static void
test_dict(size_t  n)
{
  map_ulong_t dict;
  map_ulong_init(&dict);
  
  for (size_t i = 0; i < n; i++) {
    unsigned long value = rand_get();
    unsigned long key = rand_get();
    bar_t * kyle = pottery_null;
    map_ulong_emplace_key(&dict, key, &kyle, pottery_null);
    kyle->key = key;
    kyle->value = value;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    bar_t *kyle = map_ulong_find(&dict, rand_get());
    if (kyle != NULL)
      s += kyle->value;
  }
  g_result = s;
  
  map_ulong_destroy(&dict);
}

/********************************************************************************************/

typedef struct {
  char a[256];
} char_array_t;
static inline void char_init (char_array_t *a) { a->a[0] = 0; }
static inline void char_set (char_array_t *a, const char_array_t b) { strcpy(a->a, b.a); }
static inline bool char_equal_p (const char_array_t a, const char_array_t b) { return strcmp(a.a,b.a)==0; }
static inline void char_clear (char_array_t *a) { (void)a; }
static inline size_t char_hash(const char_array_t *a) {
  size_t hash = 0;
  const char *s = a->a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

typedef struct bars_s {
  char_array_t key;
  char_array_t value;
} bars_t;

#define POTTERY_OPEN_HASH_MAP_PREFIX map_achar

#define POTTERY_OPEN_HASH_MAP_VALUE_TYPE bars_t
#define POTTERY_OPEN_HASH_MAP_KEY_TYPE char_array_t
#define POTTERY_OPEN_HASH_MAP_REF_KEY(v) v->key
#define POTTERY_OPEN_HASH_MAP_KEY_HASH(x) char_hash(&x)
#define POTTERY_OPEN_HASH_MAP_KEY_EQUAL(x, y) char_equal_p(x, y)
#define POTTERY_OPEN_HASH_MAP_LIFECYCLE_MOVE(p,q) memcpy(p, q, sizeof (bars_t))

#include "pottery/open_hash_map/pottery_open_hash_map_static.t.h"

static void
test_dict_big(size_t  n)
{
  map_achar_t dict;
  map_achar_init(&dict);

  for (size_t i = 0; i < n; i++) {
    char_array_t s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    bars_t * kyle = pottery_null;
    map_achar_emplace_key(&dict, s1, &kyle, pottery_null);
    char_set(&kyle->key, s1);
    char_set(&kyle->value, s2);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1.a, "%u", rand_get());
    bars_t *kyle = map_achar_find(&dict, s1);
    if (kyle != NULL)
      s ++;
  }
  g_result = s;

  map_achar_destroy(&dict);
}

/********************************************************************************************/

#define POTTERY_VECTOR_PREFIX vec_float
#define POTTERY_VECTOR_VALUE_TYPE float
#define POTTERY_VECTOR_LIFECYCLE_BY_VALUE 1
#include "pottery/vector/pottery_vector_static.t.h"

#define POTTERY_INTRO_SORT_PREFIX sort_float
#define POTTERY_INTRO_SORT_VALUE_TYPE float
#define POTTERY_INTRO_SORT_LIFECYCLE_MOVE_BY_VALUE 1
#define POTTERY_INTRO_SORT_COMPARE_THREE_WAY(x, y) ( *x < *y ? -1 : *x > *y)
#include "pottery/intro_sort/pottery_intro_sort_static.t.h"

static void test_sort(size_t n)
{
  vec_float_t a1;
  vec_float_init(&a1);

  for(size_t i = 0; i < n; i++) {
    vec_float_insert_last(&a1, rand_get() );
  }
  sort_float(vec_float_at(&a1, 0), n);
  g_result = *vec_float_at(&a1, 0);
  vec_float_destroy(&a1);
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
  test("POTTERY", numberof(table), table, argc, argv);
  exit(0);
}
