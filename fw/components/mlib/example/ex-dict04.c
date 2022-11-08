#include <stdio.h>
#include "m-string.h"
#include "m-dict.h"

/* program that reads text from standard-input, 
   and prints the total number of distinct words found in the text. 
*/

/* Definition of a set (hashset) of unique string_t */
DICT_SET_DEF(dict_str, string_t, STRING_OPLIST)

int main(void)
{
  // Define word as a string_t
  M_LET(word, STRING_OPLIST)
  // Define wordcount as a set of string_t
  M_LET(wordcount, DICT_SET_OPLIST(dict_str, STRING_OPLIST)) {
    // Read a word from the standard input
    while (string_fget_word(word, " \t\n,.!;:?", stdin)) {
      // Push this word in the set.
      // Do nothing if it already exists.
      dict_str_push(wordcount, word);
    }
    // Print the number of words of the standard input.
    printf ("Words: %zu\n", dict_str_size(wordcount));
  }
}
