#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "m-rbtree.h"

// Let's define one utility function.
static void *xmalloc(size_t n)
{
  void *p = malloc (n);
  if (!p) abort();
  return p;
}

// Let's define a new type.
typedef struct {
  int  key;
  char *data;
} my_type_t[1];

#define SIZE 256

// Let's define the prototypes of the methods of this new type.
void my_type_init(my_type_t x);
void my_type_clear(my_type_t x);
void my_type_init_set(my_type_t x, const my_type_t y);
void my_type_set(my_type_t x, const my_type_t y);
void my_type_init_move(my_type_t x, my_type_t y);
int my_type_cmp(const my_type_t x, const my_type_t y);
size_t my_type_hash(const my_type_t x);
void my_type_out_str(FILE *f, const my_type_t x);
bool my_type_in_str(my_type_t x, FILE *f);
void my_type_rand(my_type_t x);

// Let's define its constructor
void my_type_init(my_type_t x)
{
  x->key = 0;
  x->data = xmalloc (SIZE);
}

// Let's define its destructor
void my_type_clear(my_type_t x)
{
  free (x->data);
}

// Let's define its copy constructor
void my_type_init_set(my_type_t x, const my_type_t y)
{
  x->key = y->key;
  x->data = xmalloc (SIZE);
  memcpy(x->data, y->data, SIZE);
}

// Let's define its copy method
void my_type_set(my_type_t x, const my_type_t y)
{
  x->key = y->key;
  memcpy(x->data, y->data, SIZE);
}

// Let's define its move constructor
void my_type_init_move(my_type_t x, my_type_t y)
{
  x->key = y->key;
  x->data = y->data;
}

// Let's define its comparison method
int my_type_cmp(const my_type_t x, const my_type_t y)
{
  return (x->key < y->key) ? -1 : (x->key > y->key);
}

// Let's define its hash method
// (Not needed for tree)
size_t my_type_hash(const my_type_t x)
{
  return m_core_hash(&x->key, sizeof(x->key));
}

// Let's define its output method
void my_type_out_str(FILE *f, const my_type_t x)
{
  fprintf (f, "{ key: %d, data: ", x->key);
  for(int i = 0; i < SIZE; i++) {
    fprintf (f, "%02x", x->data[i]);
  }
  fprintf(f, "}");
}

// Let's define its input method
bool my_type_in_str(my_type_t x, FILE *f)
{
  char buffer[256];
  buffer[0] = 0;
  fgets (buffer, 8, f);
  if (strcmp (buffer, "{ key: ") != 0)
    return false;
  if (fscanf (f, "%d", &x->key) != 1)
    return false;
  fgets (buffer, 9, f);
  if (strcmp (buffer, ", data: ") != 0)
    return false;
  for(int i = 0; i < SIZE; i++) {
    fgets (buffer, 3, f);
    unsigned int n;
    if (fscanf (f, "%02x", &n) != 1)
      return false;
    x->data[i] = n;
  }
  fgets (buffer, 2, f);
  if (strcmp (buffer, "}") != 0)
    return false;
  return true;
}

void my_type_rand(my_type_t x)
{
  x->key = rand();
  for(int i = 0 ; i < SIZE; i ++) {
    x->data[i] = rand();
  }
}

// Let's define its oplist
#define MY_TYPE_OPLIST (TYPE(my_type_t), INIT(my_type_init), CLEAR(my_type_clear), INIT_SET(my_type_init_set), SET(my_type_set), INIT_MOVE(my_type_init_move), CMP(my_type_cmp), HASH(my_type_hash), OUT_STR(my_type_out_str), IN_STR(my_type_in_str))

// Let's define a Red Black Tree over this structure
RBTREE_DEF(rbtree_my_type, my_type_t, MY_TYPE_OPLIST)

int main(int argc, const char *argv[])
{
  rbtree_my_type_t tree;
  my_type_t x;  
  int n = (argc >= 2) ? atoi(argv[1]) : 10;

  rbtree_my_type_init(tree);

  // Init the tree with random values
  my_type_init(x);
  for(int i = 0 ; i < n; i++) {
    my_type_rand(x);
    rbtree_my_type_push (tree, x);
  }
  my_type_clear(x);

  // Printf the trees
  rbtree_my_type_out_str (stdout, tree);
  printf ("\n");

  // Quit
  rbtree_my_type_clear(tree);
}
