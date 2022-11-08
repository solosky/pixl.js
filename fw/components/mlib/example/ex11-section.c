#include <stdio.h>

#include "m-array.h"
#include "m-tuple.h"
#include "m-dict.h"
#include "m-string.h"

// Let's define a tuple composed of (offset, value) and register it
TUPLE_DEF2(symbol, (offset, long), (value, long))
#define M_OPL_symbol_t() TUPLE_OPLIST(symbol, M_BASIC_OPLIST, M_BASIC_OPLIST)

// Let's define an array of such tuple and register it
ARRAY_DEF(array_symbol, symbol_t)
#define M_OPL_array_symbol_t() ARRAY_OPLIST(array_symbol, M_OPL_symbol_t())

// Let's define a dictionnary associating a string_t to such an array and register it
DICT_DEF2(sections, string_t, array_symbol_t)
#define M_OPL_sections_t() DICT_OPLIST(sections, STRING_OPLIST, M_OPL_array_symbol_t())

int main(int argc, const char *argv[])
{
  if (argc < 2) abort();
  FILE *f = fopen(argv[1], "rt");
  if (!f) {
    fprintf(stderr, "ERROR: Cannot open %s\n", argv[1]);
    return 2;
  }

  // Create & initialize sc as sections_t
  M_LET(sc, sections_t) {
    // Read the section from the FILE
    sections_in_str(sc, f);
    // Get the section named .text
    array_symbol_t *a = sections_get(sc, STRING_CTE(".text"));
    if (a == NULL) {
      printf("There is no .text section.");
    } else {
      // Print its content
      printf("Section .text is :");
      array_symbol_out_str(stdout, *a);
      printf("\n");
    }
  }
  return 0;
}
