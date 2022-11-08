#include <stdio.h>
#include "m-array.h"
#include "m-algo.h"

/* Define a dynamic array of int */ 
ARRAY_DEF(vector_int, int)
/* Register the oplist of this dynamic array globally */
#define M_OPL_vector_int_t() ARRAY_OPLIST(vector_int)
/* Define some standards algorithms on this dynamic array */
ALGO_DEF(vector_int, vector_int_t)

/* Define a dynamic array of bool */ 
ARRAY_DEF(vector_bool, bool, M_BOOL_OPLIST)
/* Register the oplist of this dynamic array globally */
#define M_OPL_vector_bool_t() ARRAY_OPLIST(vector_bool, M_BOOL_OPLIST)
/* Define some standards algorithms on this dynamic array */
ALGO_DEF(vector_bool, vector_bool_t)

/* ID function */
static bool id(bool x) { return x ; }

int main(void)
{
  // Define, init (& later clear) myvector & tmp as vector_int_t
  M_LET(myvector, tmp, vector_int_t) {
    // Fill myvector with 3 int equal to 20
    vector_int_fill_n(myvector, 3, 20);
    // Fill tmp with 3 int equal to 33
    vector_int_fill_n(tmp, 3, 33);
    // Append all tmp in myvector
    vector_int_splice(myvector, tmp);
    // Fill tmp with 2 int equal to 10 
    vector_int_fill_n(tmp, 2, 10);
    // Append all tmp in myvector
    vector_int_splice(myvector, tmp);
    // Display the result
    printf("myvector contains:");
    // In C11, the for loop can be simplified as :
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
    ALGO_FOR_EACH(myvector, vector_int_t, M_PRINT, " ");
#else
    for M_EACH(it, myvector, vector_int_t) {
        printf(" %d", *it);
      }
#endif
    printf("\n");
  }
  
  // Same thing but with booleans
  M_LET(myvector, tmp, vector_bool_t) {
    vector_bool_fill_n(myvector, 3, false);
    vector_bool_fill_n(tmp, 3, true);
    vector_bool_splice(myvector, tmp);
    vector_bool_fill_n(tmp, 2, false);
    vector_bool_splice(myvector, tmp);
    printf("myvector contains:");
    for M_EACH(it, myvector, vector_bool_t) {
        printf(" %d", (int)*it);
      }
    printf("\n");
    printf ("ALL: %d ANY: %d NONE: %d\n",
            vector_bool_all_of_p(myvector, id),
            vector_bool_any_of_p(myvector, id),
            vector_bool_none_of_p(myvector, id) );
  }

  return 0;
}
