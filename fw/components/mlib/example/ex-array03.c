#include <stdlib.h>
#include "m-array.h"

// Let's define an array of 'int' named 'array_int'
ARRAY_DEF(array_int, int)

const int n = 100000;

int main(void)
{
  // Declare and initialize the array
  array_int_t x;
  array_int_init(x);

  // Push some integers in the array
  for (int i = 0; i < n; i++) {
    array_int_push_back(x, rand());
  }

  // Insert some integers in the array.
  for (int i = 0; i < n; i++) {
    array_int_push_at (x, rand() % n, rand());
  }

  // Pop some integers from integer into NULL,
  // i.e. erase them.
  for (int i = 0; i < n; i++) {
    array_int_pop_at (NULL, x, rand() % n);
  }

  // Clear the array
  array_int_clear(x);
  return 0;
}
