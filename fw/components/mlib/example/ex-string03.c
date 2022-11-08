#include <stdio.h>
#include "m-string.h"
#include "m-dict.h"
#include "m-array.h"

/* Definition of an associative map string_t --> size_t */
DICT_DEF2(dict_str, string_t, size_t)
#define M_OPL_dict_str_t() DICT_OPLIST(dict_str, STRING_OPLIST, M_BASIC_OPLIST)

/* Definition of an array of string_t */
ARRAY_DEF(vector_str, string_t)
#define M_OPL_vector_str_t() ARRAY_OPLIST(vector_str, STRING_OPLIST)

int main(void)
{
    // Construct an array of string, performing a convertion of the C const char *
    // into a proper string_t at real time and push then into a dynamic array
    // that is declared, initialized and cleared.
    M_LET( (words, ("This"), ("is"), ("a"), ("useless"), ("sentence"), ("."),
                    ("It"), ("is"), ("used"), ("a"), ("bit"), ("to"), ("count"), ("words"), (".") ),
                    vector_str_t) {
        // Print the arrays.
        printf("The words are: ");
        vector_str_out_str(stdout, words);
        printf("\n");

        // Count the words.
        M_LET(map, dict_str_t) {
            // Count the words:
            for M_EACH(w, words, vector_str_t) {
                (*dict_str_safe_get(map, *w)) ++;
            }

            // Print the count:
            for M_EACH(p, map, dict_str_t) {
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)            
                M_PRINT(p->value, " occurences of ", p->key, "\n");
#else
                printf ("%zu occurences of %s\n", p->value, string_get_cstr(p->key));
#endif
            }
        }
    }
}
