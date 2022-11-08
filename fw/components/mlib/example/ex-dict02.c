#include "m-dict.h"

// Let's define a dictionary of 'unsigned int' --> 'char
DICT_DEF2(dict_m32, unsigned int, M_BASIC_OPLIST, char, M_BASIC_OPLIST)

// Let's create a synonym for its oplist.
#define M32_OPLIST DICT_OPLIST(dict_m32, M_BASIC_OPLIST, M_BASIC_OPLIST)

#if 1 // Non macro version.

int main(void) {
  dict_m32_t h;                             // Declare h as a dictionary
  dict_m32_init(h);                         // h is initialized.
  dict_m32_set_at (h, 5, 10);               // h[5] = 10
  char *k = dict_m32_get(h, 10);            // No 10 in h, so k == NULL
  int is_missing = (k != NULL);             // true
  assert (is_missing); (void) is_missing;
  dict_m32_erase(h, 5);                    // h is now empty
  dict_m32_it_t it;                         // iterate over all dictionnary
  for (dict_m32_it (it, h) ; !dict_m32_end_p (it); dict_m32_next(it)) {
    struct dict_m32_pair_s *item = dict_m32_ref(it);
    item->value = 1;                        // Set its value to 1
  }
  dict_m32_clear(h);                        // h is cleared
  return 0;
}

#else // Use of M_LET & M_FOR macros (otherwise equivalent)

int main(void) {
  M_LET(h, M32_OPLIST) {                      // h is init
    dict_m32_set_at (h, 5, 10);               // h[5] = 10
    char *k = dict_m32_get(h, 10);            // k == NULL
    int is_missing = (k != NULL);             // true
    assert (is_missing);
    dict_m32_erase(h, 5);                    // h is now empty
    for M_EACH(item, h, M32_OPLIST) {         // traverse each item
        item->value = 1;                      // Set its value to 1
      }
  }                                           // h is cleared
  return 0;
}

#endif

     
