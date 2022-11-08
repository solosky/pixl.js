#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <glib.h>

#include "common.h"

/********************************************************************************************/

static void test_array(size_t n)
{
  GArray *a1, *a2;
  a1 = g_array_new (FALSE, FALSE, sizeof (unsigned int));
  a2 = g_array_new (FALSE, FALSE, sizeof (unsigned int));
  for(size_t i = 0; i < n; i++) {
    unsigned int v = rand_get();
    g_array_append_val(a1, v );
    v = rand_get();
    g_array_append_val(a2, v );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += g_array_index(a1, unsigned int, i ) * g_array_index(a2, unsigned int, i );
  }
  g_result = s;
  g_array_free(a1, FALSE);
  g_array_free(a2, FALSE);
}

/********************************************************************************************/

static void test_list (size_t n)
{
  GSList *a1 = NULL, *a2 = NULL;

  for(size_t i = 0; i < n; i++) {
    a1 = g_slist_prepend (a1, GINT_TO_POINTER (rand_get() ));
    a2 = g_slist_prepend (a2, GINT_TO_POINTER (rand_get() ));
  }
  unsigned int s = 0;
  for(GSList *it1 = a1, *it2 = a2 ; it1 != NULL; it1 = g_slist_next(it1), it2 = g_slist_next(it2) ) {
    s += GPOINTER_TO_INT(it1->data) * GPOINTER_TO_INT(it2->data);
    n--;
  }
  g_result = s;
  g_slist_free(a1);
  g_slist_free(a2);
}

/********************************************************************************************/

static int compare(gconstpointer a, gconstpointer b, gpointer user_data)
{
  const unsigned long *pa = a;
  const unsigned long *pb = b;
  (void) user_data;
  return (*pa < *pb) ? -1 : (*pa > *pb);
}

static void test_rbtree(size_t n)
{
  GTree *tree = g_tree_new_full(compare, NULL, free, NULL);
  for (size_t i = 0; i < n; i++) {
    // NOTE: unsigned long are not designed to work with GPOINTER_TO_INT according to documentation.
    unsigned long *p = malloc(sizeof(unsigned long));
    *p = rand_get();
    g_tree_insert(tree, p, p);
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long key = rand_get();
    unsigned long *p = g_tree_lookup(tree, &key);
    if (p)
      s += *p;
  }
  g_result = s;
  g_tree_destroy(tree);
}

/********************************************************************************************/
static guint hash_func (gconstpointer a)
{
  const unsigned long *pa = a;  
  return *pa;
}

static gboolean equal_func(gconstpointer a, gconstpointer b)
{
  const unsigned long *pa = a;
  const unsigned long *pb = b;
  return *pa == *pb;
}
          
static void
test_dict(size_t  n)
{
  GHashTable *dict = g_hash_table_new_full(hash_func, equal_func, free, free);
  for (size_t i = 0; i < n; i++) {
    unsigned long *key = malloc(sizeof(unsigned long));
    unsigned long *value = malloc(sizeof(unsigned long));
    *key = rand_get();
    *value = rand_get();   
    g_hash_table_insert(dict, key, value );
  }    
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    unsigned long key = rand_get();
    unsigned long *p = g_hash_table_lookup(dict, &key);
    if (p)
      s += *p;
  }
  g_result = s;
  g_hash_table_destroy(dict);
}

/********************************************************************************************/

typedef char char_array_t[256];

static gboolean char_equal (gconstpointer a, gconstpointer b)
{
  const char_array_t *pa = (const char_array_t *)a;
  const char_array_t *pb = (const char_array_t *)b;
  return strcmp(*pa,*pb)==0;
}

static guint char_hash(gconstpointer a)
{
  const char_array_t *pa = (const char_array_t *)a;
  const char *s = *pa;
  guint hash = 0;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
}

static void
test_dict_big(size_t n)
{
  GHashTable *dict = g_hash_table_new_full(char_hash, char_equal, free, free); 
  for (size_t i = 0; i < n; i++) {
    char_array_t *key = malloc (sizeof (char_array_t));
    char_array_t *value = malloc (sizeof (char_array_t));
    sprintf(*key, "%u", rand_get());
    sprintf(*value, "%u", rand_get());
    g_hash_table_insert(dict, key, value );
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_t s1;
    sprintf(s1, "%u", rand_get());
    char_array_t *p = g_hash_table_lookup(dict, &s1);
    if (p)
      s ++;
  }
  g_result = s;
  g_hash_table_destroy(dict);
}

/********************************************************************************************/

static gint cmp_float(gconstpointer a,
                      gconstpointer b)
{
  const float *pa = a, *pb = b;
  return (*pa < *pb) ? -1 : (*pa > *pb);
}

static void test_sort(size_t n)
{
  GArray *a1 = g_array_new (FALSE, FALSE, sizeof (float));
  for(size_t i = 0; i < n; i++) {
    float v = rand_get();
    g_array_append_val(a1, v );
  }
  g_array_sort(a1, cmp_float);
  g_result = g_array_index (a1, float, 0);
  g_array_free(a1, FALSE);
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
  test("GLIB", numberof(table), table, argc, argv);
  exit(0);
}
