// Based upon http://www.sanfoundry.com/cpp-program-implement-map-stl/
#include <stdio.h>
#include <stdlib.h>

#include "m-dict.h"
#include "m-string.h"

// Define a dictionary (hashmap) from 'char' to 'int'
DICT_DEF2(dict_map, char, int)
             
int main(void)
{
  dict_map_t mp;
  dict_map_it_t it;
  int choice, item;
  string_t str;
  char s;

  dict_map_init(mp);
  string_init (str);
  
  while (true)
    {
      printf ("\n---------------------\n");
      printf ("Map Implementation in M*LIB\n");
      printf ("\n---------------------\n");
      printf("1.Insert Element into the Map\n");
      printf("2.Delete Element of the Map\n");
      printf("3.Size of the Map\n");
      printf("4.Find Element at a key in Map\n");
      printf("5.Display by Iterator\n");
      printf("6.Exit\n");
      printf("Enter your Choice: ");

      string_fgets (str, stdin, STRING_READ_PURE_LINE);
      choice = atoi(string_get_cstr(str));

      switch(choice)
        {
        case 1:
          printf("Enter value to be inserted: ");
          string_fgets (str, stdin, STRING_READ_PURE_LINE);
          item = atoi(string_get_cstr(str));
          printf("Enter the key: ");
          string_fgets (str, stdin, STRING_READ_PURE_LINE);
          s = atoi(string_get_cstr(str));
          dict_map_set_at(mp, s, item);
          break;

        case 2:
          printf("Enter the mapped string to be deleted: ");
          string_fgets (str, stdin, STRING_READ_PURE_LINE);
          s = atoi(string_get_cstr(str));
          dict_map_erase(mp, s);
          break;

        case 3:
          printf("Size of Map: %zu\n", dict_map_size(mp) );
          break;

        case 4:
          printf("Enter the key at which value to be found: ");
          string_fgets (str, stdin, STRING_READ_PURE_LINE);
          s = atoi(string_get_cstr(str));
          int *ref = dict_map_get(mp, s);
          if (ref != NULL)
            {
              printf("Found %d\n", *ref);
            }
          else
            {
              printf("No Element Found\n");
            }
          break;

        case 5:
          printf("Displaying Map by Iterator:\n");
          for (dict_map_it(it, mp); !dict_map_end_p(it); dict_map_next(it))
            {
              const struct dict_map_pair_s *cref = dict_map_cref(it);
              printf ("%d : %d\n", cref->key, cref->value);
            }
          break;

        case 6:
          dict_map_clear(mp);
          string_clear (str);
          exit(0);
          break;
          
        default:
          printf("Wrong Choice\n");
          break;
        }     
    }
}
