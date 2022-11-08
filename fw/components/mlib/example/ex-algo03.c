#include "m-dict.h"
#include "m-array.h"
#include "m-algo.h"
#include "m-string.h"

/* Define a dictionnary (or hash_map or unordered_map)
 * from a string_t to int */
DICT_DEF2(dict, string_t, int)
/* Register the oplist of this dictionnary globally */
#define M_OPL_dict_t() DICT_OPLIST(dict, STRING_OPLIST, M_BASIC_OPLIST)

/* Define an array of integer */
ARRAY_DEF(vector_int, int)
/* Register the oplist of this array of integer globally */
#define M_OPL_vector_int_t() ARRAY_OPLIST(vector_int)

/* Micro macro to detect if a string starts with the given pattern */
#define start_with(pattern, item)                \
  string_start_with_str_p((item).key, (pattern))

/* Micro macro to get the value of a pair of (key, value) */
#define get_value(out, item) ((out) = (item).value)

int main(void)
{
  int s = 0;
  // Init keys as a vector_int_t, initialize it (and prepare its destruction)
  M_LET(keys, vector_int_t)
    // Initialize m as a dictionnary and initialize it with the given database
    // Initialize tmp as a dictionanry
    M_LET( (m, (STRING_CTE("foo"), 1), (STRING_CTE("bar"), 42), (STRING_CTE("bluez"), 7), (STRING_CTE("stop"), 789) ), tmp, dict_t) {
 
    /* Extract all elements of 'm' that starts with 'b' */
    ALGO_EXTRACT(tmp, dict_t, m, dict_t, start_with, "b");
    /* Extract the values of theses elements */
    ALGO_TRANSFORM(keys, vector_int_t, tmp, dict_t, get_value);
    /* Sum theses values */
    ALGO_REDUCE(s, keys, vector_int_t, sum);
    printf("Sum of elements starting with 'b' is: %d\n", s);
  }
  return 0;
}
