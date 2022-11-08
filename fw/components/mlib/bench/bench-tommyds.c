#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "tommyds/tommy.h"

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  tommy_array a1, a2;

  tommy_array_init(&a1);
  tommy_array_init(&a2);
  for(size_t i = 0; i < n; i++) {
    unsigned int v = rand_get();
    tommy_array_insert(&a1, (void*)(uintptr_t) v);
    v = rand_get();
    tommy_array_insert(&a2, (void*)(uintptr_t) v);
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += (uintptr_t) *tommy_array_ref(&a1, i) * (uintptr_t) *tommy_array_ref(&a2, i );
  }
  g_result = s;
  tommy_array_done(&a1);
  tommy_array_done(&a2);
}

/********************************************************************************************/

struct list_node_s {
  unsigned int value;
  tommy_node node;
};

static void test_list (size_t n)
{
  tommy_list a1, a2;

  tommy_list_init(&a1);
  tommy_list_init(&a2);
  for(size_t i = 0; i < n; i++) {
    struct list_node_s *obj;
    obj = malloc(sizeof(struct list_node_s));
    if (obj == NULL) abort();
    obj->value = rand_get();
    tommy_list_insert_tail(&a1, &obj->node, obj);
    obj = malloc(sizeof(struct list_node_s));
    if (obj == NULL) abort();
    obj->value = rand_get();
    tommy_list_insert_tail(&a2, &obj->node, obj);
  }
  unsigned int s = 0;
  for(tommy_node *it1 = tommy_list_head(&a1), *it2 = tommy_list_head(&a2)
	; it1 != NULL; it1 = it1->next, it2 = it2->next ) {
    struct list_node_s *obj1 = it1->data, *obj2 = it2->data;
    s += obj1->value * obj2->value;
  }
  g_result = s;
  tommy_list_foreach(&a1, free);
  tommy_list_foreach(&a2, free);
}

/********************************************************************************************/

struct tree_node_s {
  unsigned long value;
  tommy_tree_node node;
} ;

static int compare(const void *a, const void *b)
{
  const struct tree_node_s *pa = a;
  const struct tree_node_s *pb = b;
  return (pa->value < pb->value) ? -1 : (pa->value > pb->value);
}

static void test_rbtree(size_t n)
{
  tommy_tree tree;
  tommy_tree_init(&tree, compare);
  for (size_t i = 0; i < n; i++) {
    struct tree_node_s *obj = malloc(sizeof(struct tree_node_s));
    if (obj == NULL) abort();
    obj->value = rand_get();
    tommy_tree_insert(&tree, &obj->node, obj);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    struct tree_node_s key = { .value = rand_get() };
    struct tree_node_s *obj = tommy_tree_search (&tree, &key);
    if (obj)
      s += obj->value;
  }
  g_result = s;
  tommy_tree_foreach(&tree, free);
}

/********************************************************************************************/

struct hash_i_node_s {
  unsigned long key;
  unsigned long value;
  tommy_node node;
};

static inline size_t hash_func (const unsigned long key)
{
  return key;
}

static int equal_func(const void* arg, const void *obj_p)
{
  const struct hash_i_node_s *obj = obj_p;
  const unsigned long *key = arg;
  return *key != obj->key;
}

static void
test_dict(size_t  n)
{
  tommy_hashlin dict;
  tommy_hashlin_init(&dict);
  for (size_t i = 0; i < n; i++) {
    struct hash_i_node_s *obj = malloc(sizeof (struct hash_i_node_s));
    if(!obj) abort();
    obj->key = rand_get();
    obj->value = rand_get();
    tommy_hashlin_insert(&dict, &obj->node, obj, hash_func(obj->key));
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long key = rand_get();
    struct hash_i_node_s *obj = tommy_hashlin_search(&dict, &equal_func,
						      &key,
						      hash_func(key));
    if (obj)
      s += obj->value;
  }
  g_result = s;
  tommy_hashlin_done(&dict);
}

/********************************************************************************************/

typedef char char_array_t[256];

struct hash_b_node_s {
  char_array_t key;
  char_array_t value;
  tommy_node node;
};

static int char_equal (const void *arg, const void *obj_p)
{
  const struct hash_b_node_s *obj = obj_p;
  const char_array_t *key = (const char_array_t *)arg;
  return strcmp(*key, (obj->key));
}

static size_t char_hash(const char *s)
{
  size_t hash = 0;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

static void
test_dict_big(size_t  n)
{
  tommy_hashlin dict;
  tommy_hashlin_init(&dict);
  for (size_t i = 0; i < n; i++) {
    struct hash_b_node_s *obj = malloc(sizeof (struct hash_b_node_s));
    if(!obj) abort();
    sprintf(obj->key, "%u", rand_get());
    sprintf(obj->value, "%u", rand_get());
    /* NOTE: this is not an insert_or_replace func...
       It will always insert a new node, even if the key already exists...
       Maybe searching then inserting will be a better representative?
    */
    tommy_hashlin_insert(&dict, &obj->node, obj, char_hash(obj->key));
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1, "%u", rand_get());
    struct hash_b_node_s *obj = tommy_hashlin_search(&dict, &char_equal,
						      &s1,
						      char_hash(s1));
    if (obj)
      s ++;
  }
  g_result = s;
  tommy_hashlin_done(&dict);
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0},
  { 40,    "dict", 1000000, 0, test_dict, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0}
};

int main(int argc, const char *argv[])
{
  test("TommyDS", numberof(table), table, argc, argv);
  exit(0);
}
