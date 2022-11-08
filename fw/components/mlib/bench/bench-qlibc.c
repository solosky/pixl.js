#define NDEBUG
#define _GNU_SOURCE 

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "qlibc/qlibc.h"

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  qvector_t *a1 = qvector(16, sizeof(unsigned int), QVECTOR_RESIZE_DOUBLE);
  if (!a1) abort();
  qvector_t *a2 = qvector(16, sizeof(unsigned int), QVECTOR_RESIZE_DOUBLE);
  if (!a2) abort();

  for(size_t i = 0; i < n; i++) {
    unsigned int v = rand_get();
    bool success = a1->addlast(a1, &v);
    if (!success) abort();
    v = rand_get();
    success = a2->addlast(a2, &v);
    if (!success) abort();
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += *(unsigned int*)a1->getat(a1, i, false) * *(unsigned int *) a2->getat(a2, i, false);
  }
  g_result = s;
  a1->free(a1);
  a2->free(a2);
}

/********************************************************************************************/

static void test_list (size_t n)
{
  qlist_t *a1 = qlist(0);
  qlist_t *a2 = qlist(0);

  for(size_t i = 0; i < n; i++) {
    unsigned int u = rand_get(), v = rand_get();
    a1->addlast(a1, &u, sizeof(unsigned int));
    a2->addlast(a2, &v, sizeof(unsigned int));
  }
  unsigned int s = 0;
  qlist_obj_t it1, it2;
  memset(&it1, 0, sizeof (it1));
  memset(&it2, 0, sizeof (it2));
  while (a1->getnext(a1, &it1, false) == true
	 && a2->getnext(a2, &it2, false) == true) {
    s += *(unsigned int*)it1.data * *(unsigned int*)it2.data;
  }
  g_result = s;
  a1->free(a1);
  a2->free(a2);
}

/********************************************************************************************/

static int compare(const void *key1, size_t size1,
		   const void *key2, size_t size2)
{
  assert(size1 == sizeof(unsigned long));
  assert(size2 == sizeof(unsigned long));
  const unsigned long *pa = key1;
  const unsigned long *pb = key2;
  return (*pa < *pb) ? -1 : (*pa > *pb);
}

static void test_rbtree(size_t n)
{
  qtreetbl_t *tree = qtreetbl(0);
  tree->set_compare(tree, compare);
  for (size_t i = 0; i < n; i++) {
    unsigned long v = rand_get();
    tree->put_by_obj(tree,
		     &v, sizeof(unsigned long), /* key */
		     &v, sizeof(unsigned long) /* value */ );
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long v = rand_get();
    void *obj = tree->get_by_obj(tree, (const char*)&v, sizeof(unsigned long), NULL, 0);
    if (obj)
      s += *(unsigned long*)obj;
  }
  g_result = s;
  tree->free(tree);
}

/* qlibc hash functions only accept a string as the key */

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0}
};

int main(int argc, const char *argv[])
{
  test("QLIBC", numberof(table), table, argc, argv);
  exit(0);
}
