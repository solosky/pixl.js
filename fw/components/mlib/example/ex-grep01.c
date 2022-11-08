#include <stdio.h>
#include "m-string.h"
#include "m-array.h"
#include "m-dict.h"

// Define an array of size_t
ARRAY_DEF(array_size, size_t)
// Register the oplist of this array globaly
#define M_OPL_array_size_t() ARRAY_OPLIST(array_size)

// Define a dictionary (hashmap) of string_t to array of size_t
DICT_DEF2(map_pos, string_t, array_size_t)
// Register the oplist of this dictionary (hashmap)
#define M_OPL_map_pos_t() DICT_OPLIST(map_pos, STRING_OPLIST, ARRAY_OPLIST(array_size))

int main(int argc, const char *argv[])
{
   if (argc != 2) {
      printf("USAGE: %s filename\n", argv[0]);
      exit(0);
   }

   // Define word as a string_t
   M_LET(word, string_t)
   // Define positions as a map of string_t-->array of size_t
   M_LET(positions, map_pos_t) {
      // Open the file
      FILE *f = fopen(argv[1], "rt");
      if (!f) {
         fprintf(stderr, "ERROR: Cannot open %s.\n", argv[1]);
         return 1;
      }

      // While it reads some word from the file
      while (string_fget_word(word, " \t\n\r\"(),=", f)) {
         // Get the reference to the array of this word in the dictionnary (or create it if needed)
         array_size_t *array = map_pos_safe_get(positions, word);
         // Push the new offset to this word in the associated array
         array_size_push_back(*array, (size_t)ftell(f) - string_size(word));
      }

      // Close the file
      fclose(f);

      // Print the words and where they are:
      for M_EACH(pair, positions, map_pos_t) {
         string_fputs(stdout, pair->key);
         for M_EACH(pos, pair->value, array_size_t) {
            printf(" %zu", *pos);
         }
         printf("\n");
         }
      } // All created objects are destroyed beyond this point

   return 0;
}
