/* Based on https://en.cppreference.com/w/cpp/algorithm/for_each
   Need to be built in C11 mode */

#include <stdio.h>
#include "m-array.h"
#include "m-algo.h"

/* Define a dynamic array of int */ 
ARRAY_DEF(vector_int, int)
#define M_OPL_vector_int_t() ARRAY_OPLIST(vector_int)

/* Define operator increment on int */
#define INC(n) (n)++

int main(void)
{
  M_LET( (nums, 3, 4, 2, 8, 15, 267), vector_int_t) {
    // Print the array before
    printf ("before:");
    ALGO_FOR_EACH(nums, vector_int_t, M_PRINT, " ");
    printf ("\n");

    // Increment each element of the array
    ALGO_FOR_EACH(nums, vector_int_t, INC);
    
    // Print the array after
    printf ("after: ");
    ALGO_FOR_EACH(nums, vector_int_t, M_PRINT, " ");
    printf ("\n");

    // Sum the elements of the array
    int sum = 0;
    ALGO_REDUCE(sum, nums, vector_int_t, add);
    M_PRINT("sum = ", sum, "\n");
  }
  return 0;
}
