#define NDEBUG
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "nedtrie.h"

#include "common.h"

/********************************************************************************************/

typedef struct foo_s {
  NEDTRIE_ENTRY(foo_s) link;
  unsigned long key;
  unsigned long value;
} foo_t;
typedef struct foo_tree_s foo_tree_t;
NEDTRIE_HEAD(foo_tree_s, foo_s);

static inline size_t fookeyfunct(const foo_t *r)
{
  return r->key;
}

NEDTRIE_GENERATE(static, foo_tree_s, foo_s, link, fookeyfunct, NEDTRIE_NOBBLEZEROS(foo_tree_s))

static void
test_dict(size_t  n)
{
  foo_tree_t footree;
  NEDTRIE_INIT(&footree);
  
  for (size_t i = 0; i < n; i++) {
    unsigned long key = rand_get();
    unsigned long val = rand_get();
    foo_t * p = malloc(sizeof(foo_t));
    if (p == NULL) abort();
    p->key = key;
    p->value = val;
    NEDTRIE_INSERT(foo_tree_s, &footree, p);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    foo_t key = { .key = rand_get() };
    foo_t *p = NEDTRIE_FIND(foo_tree_s, &footree, &key);
    if (p)
      s += p->value;
  }
  g_result = s;
  foo_t *r, *next;
  NEDTRIE_FOREACH_SAFE(r, foo_tree_s, &footree, next) {
    // Remove is needed, otherwise it segfault
    NEDTRIE_REMOVE(foo_tree_s, &footree, r);
    free(r);
  }
}

static void
test_dict_noalloc(size_t  n)
{
  foo_tree_t footree;
  NEDTRIE_INIT(&footree);

  foo_t *tab = malloc(n*sizeof(foo_t));
  if (tab == NULL) abort();
  
  for (size_t i = 0; i < n; i++) {
    unsigned long key = rand_get();
    unsigned long val = rand_get();
    tab[i].key = key;
    tab[i].value = val;
    NEDTRIE_INSERT(foo_tree_s, &footree, &tab[i]);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    foo_t key = { .key = rand_get() };
    foo_t *p = NEDTRIE_FIND(foo_tree_s, &footree, &key);
    if (p)
      s += p->value;
  }
  g_result = s;
  free(tab);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 45, "dict(noalloc)", 1000000, 0, test_dict_noalloc, 0}
};

int main(int argc, const char *argv[])
{
  test("NEDTRIES", numberof(table), table, argc, argv);
  exit(0);
}
