#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "kvec.h"
#include "klist.h"
#include "kbtree.h"
#include "khash.h"
#include "ksort.h"

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  kvec_t(unsigned int) a1, a2;

  kv_init(a1);
  kv_init(a2);
  
  for(size_t i = 0; i < n; i++) {
    kv_push(unsigned int, a1, rand_get() );
    kv_push(unsigned int, a2, rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += kv_A(a1, i ) * kv_A(a2, i );
  }
  g_result = s;

  kv_destroy(a1);
  kv_destroy(a2);
}

/********************************************************************************************/

#define uint_free(x)
KLIST_INIT(list_uint, unsigned int, uint_free)

static void test_list (size_t n)
{
  klist_t(list_uint) *a1, *a2;
  a1 = kl_init(list_uint);
  a2 = kl_init(list_uint);

  for(size_t i = 0; i < n; i++) {
    *kl_pushp (list_uint, a1) = rand_get();
    *kl_pushp (list_uint, a2) = rand_get();
  }
  unsigned int s = 0;
  kliter_t(list_uint) *it1, *it2;
  for(it1 = kl_begin(a1), it2 = kl_begin(a2);
      it1 != kl_end(a1);
      it1 = kl_next(it1), it2 = kl_next(it2)) {
    s += kl_val(it1) * kl_val(it2);
  }
  g_result = s;

  kl_destroy(list_uint, a1);
  kl_destroy(list_uint, a2);
}

/********************************************************************************************/

#define my_cmp(a,b) ( (a) < (b) ? -1 : (a) > (b) )

KBTREE_INIT(kTree, unsigned long, my_cmp)

static void test_rbtree(size_t n)
{
  kbtree_t(kTree) *tree;
  tree = kb_init(kTree, KB_DEFAULT_SIZE/2);
  
  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    kb_putp(kTree, tree, &j);
  }
    
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long j = rand_get();
    if (kb_getp(kTree, tree, &j) != 0)
      s+= j;
  }
  g_result = s;
  kb_destroy(kTree, tree);
}

/********************************************************************************************/

#define hash_eq(a, b) ((a) == (b))
#define hash_func(a) kh_int64_hash_func(a)

KHASH_INIT(iun, unsigned long, unsigned long, 1, hash_func, hash_eq)

static void
test_dict(size_t  n)
{
  khash_t(iun) *dict = kh_init(iun);
  
  for (size_t i = 0; i < n; i++) {
    int ret;
    khiter_t k = kh_put(iun, dict, rand_get(), &ret);
    kh_value(dict, k) = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    khiter_t k = kh_get(iun, dict, rand_get());
    if (kh_exist(dict, k))
      s += kh_value(dict, k);
  }
  g_result = s;
  
  kh_destroy(iun, dict);
}

/********************************************************************************************/

typedef struct {
  char a[256];
} char_array_t;
static inline void char_init (char_array_t *a) { a->a[0] = 0; }
static inline void char_set (char_array_t *a, const char_array_t b) { strcpy(a->a, b.a); }
static inline bool char_equal_p (const char_array_t a, const char_array_t b) { return strcmp(a.a,b.a)==0; }
static inline size_t char_hash(const char_array_t a) {
  size_t hash = 0;
  const char *s = a.a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

KHASH_INIT(iub, char_array_t, char_array_t, 1, char_hash, char_equal_p)

static void
test_dict_big(size_t  n)
{
  khash_t(iub) *dict = kh_init(iub);

  for (size_t i = 0; i < n; i++) {
    int ret;
    char_array_t s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    khiter_t k = kh_put(iub, dict, s1, &ret);
    char_set(&kh_value(dict, k), s2);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1.a, "%u", rand_get());
    khiter_t k = kh_get(iub, dict, s1);
    if (kh_exist(dict, k))
      s ++;
  }
  g_result = s;

  kh_destroy(iub, dict);
}

/********************************************************************************************/
// NOTE: Needed for compiling
double drand48(void) { return rand_get(); }

KSORT_INIT_GENERIC(float)

static void test_sort(size_t n)
{
  kvec_t(float) a1;

  kv_init(a1);
  for(size_t i = 0; i < n; i++) {
    kv_push(float, a1, rand_get() );
  }
  // NOTE: Is-it the right way to do? Seems very low level!
  ks_introsort_float(n, & kv_A(a1, 0));
  g_result = kv_A(a1, 0);
  kv_destroy(a1);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
  { 50,           "Sort",10000000, 0, test_sort, 0},
};

int main(int argc, const char *argv[])
{
  test("KLIB", numberof(table), table, argc, argv);
  exit(0);
}

