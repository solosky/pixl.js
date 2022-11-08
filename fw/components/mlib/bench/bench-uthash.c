#define NDEBUG
#define  _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "include/uthash.h"
#include "include/utarray.h"
#include "include/utlist.h"

#include "common.h"

/********************************************************************************************/

static const UT_icd int_icd = {
  .sz = sizeof (unsigned int),
  .init = NULL,
  .dtor = NULL,
  .copy = NULL
};

static void test_array(size_t n)
{
  UT_array a1, a2;

  utarray_init(&a1, &int_icd);
  utarray_init(&a2, &int_icd);
  
  for(size_t i = 0; i < n; i++) {
    unsigned int v1 = rand_get(), v2 = rand_get();
    utarray_push_back(&a1, &v1);
    utarray_push_back(&a2, &v2);
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *(unsigned long *) utarray_eltptr(&a1, i ) * *(unsigned long *)utarray_eltptr(&a2, i );
  }
  g_result = s;

  utarray_done(&a1);
  utarray_done(&a2);
}

/********************************************************************************************/

struct list_node_s {
  unsigned int value;
  struct list_node_s *next;
};

static void test_list (size_t n)
{
  struct list_node_s *a1 = NULL, *a2 = NULL;

  for(size_t i = 0; i < n; i++) {
    struct list_node_s *p = malloc(sizeof(struct list_node_s));
    if (!p) abort();
    p->value = rand_get();
    LL_PREPEND(a1, p);
    p = malloc(sizeof(struct list_node_s));
    if (!p) abort();
    p->value = rand_get();
    LL_PREPEND(a2, p);
  }
  unsigned int s = 0;
  for(struct list_node_s *it1 = a1, *it2 = a2
	; it1 != NULL
	; it1 = it1->next, it2 = it2->next) {
    s += it1->value * it2->value;
  }
  g_result = s;

  struct list_node_s *el;
  LL_FOREACH(a1, el) free(el);
  LL_FOREACH(a2, el) free(el);
}

/********************************************************************************************/

struct hash_i_node_s {
  unsigned long key;
  unsigned long value;
  UT_hash_handle hh;
};

static void
test_dict(size_t  n)
{
  struct hash_i_node_s *dict = NULL;
  
  for (size_t i = 0; i < n; i++) {
    unsigned long k = rand_get();
    struct hash_i_node_s *s;
    HASH_FIND(hh, dict, &k, sizeof (unsigned long), s);
    if (s == NULL) {
      s = malloc(sizeof(struct hash_i_node_s));
      if (!s) abort();
      s->key = k;
      s->value = rand_get();
      HASH_ADD(hh, dict, key, sizeof(unsigned long), s);
    }
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long k = rand_get();
    struct hash_i_node_s *sp;
    HASH_FIND(hh, dict, &k, sizeof (unsigned long), sp);
    if (sp != NULL)
      s += sp->value;
  }
  g_result = s;
  struct hash_i_node_s *current, *tmp;
  HASH_ITER(hh, dict, current, tmp) {
    HASH_DEL(dict,current);  /* delete it (users advances to next) */
    free(current);            /* free it */
  }
}

/********************************************************************************************/

typedef struct {
  char a[256];
} char_array_t;

struct hash_b_node_s {
  char_array_t key;
  char_array_t value;
  UT_hash_handle hh;
};

static void
test_dict_big(size_t  n)
{
  struct hash_b_node_s *dict = NULL;

  for (size_t i = 0; i < n; i++) {
    char_array_t k;
    sprintf(k.a, "%u", rand_get());
    struct hash_b_node_s *s;
    HASH_FIND(hh, dict, &k, sizeof (char_array_t), s);
    if (s == NULL) {
      s = malloc(sizeof(struct hash_b_node_s));
      if (!s) abort();
      memcpy(s->key.a, k.a, sizeof(s->key));
      sprintf(s->value.a, "%u", rand_get());
      HASH_ADD(hh, dict, key, sizeof(char_array_t), s);
    }
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t k;
    sprintf(k.a, "%u", rand_get());
    struct hash_b_node_s *sp;
    HASH_FIND(hh, dict, &k, sizeof (char_array_t), sp);
    if (sp != NULL)
      s ++;
  }
  g_result = s;

  struct hash_b_node_s *current, *tmp;
  HASH_ITER(hh, dict, current, tmp) {
    HASH_DEL(dict,current);  /* delete it (users advances to next) */
    free(current);            /* free it */
  }
}

/********************************************************************************************/

static const UT_icd float_icd = {
  .sz = sizeof (float),
  .init = NULL,
  .dtor = NULL,
  .copy = NULL
};

static int cmp(const void *a1, const void *a2)
{
  const float *f1 = a1;
  const float *f2 = a2;
  return (*f1 < *f2) ? -1 : (*f1 > *f2);
}

static void test_sort(size_t n)
{
  UT_array a1;

  utarray_init(&a1, &float_icd);
  for(size_t i = 0; i < n; i++) {
    float v1 = rand_get();
    utarray_push_back(&a1, &v1);
  }
  utarray_sort(&a1, cmp);
  g_result = *(float *)utarray_eltptr(&a1, 0);
  utarray_done(&a1);  
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
  { 50,    "sort", 10000000, 0, test_sort, 0}
};

int main(int argc, const char *argv[])
{
  test("UT-HASH", numberof(table), table, argc, argv);
  exit(0);
}
