#include <stdio.h>
#include <stdint.h>

#include "m-array.h"
#include "m-dict.h"
#include "m-string.h"
#include "m-algo.h"

/* Based on:
 *  Easy Perfect Minimal Hashing 
 *  By Steve Hanov. Released to the public domain.
 *  http://stevehanov.ca/blog/index.php?id=119
 *
 * Based on:
 * Edward A. Fox, Lenwood S. Heath, Qi Fan Chen and Amjad M. Daoud, 
 * "Practical minimal perfect hash functions for large databases", CACM, 35(1):105-121
 *
 * also a good reference:
 *  Compress, Hash, and Displace algorithm by Djamal Belazzougui,
 * Fabiano C. Botelho, and Martin Dietzfelbinger
 */



/* Define the needed data structure : */

// Define a dictionnary of constant string --> integer and register it.
DICT_DEF2(dict_mph, const char *, M_CSTR_OPLIST, uint32_t, M_BASIC_OPLIST)
#define M_OPL_dict_mph_t() DICT_OPLIST(dict_mph, M_CSTR_OPLIST, M_BASIC_OPLIST)

// Define an array of integer to store the value and register it.
ARRAY_DEF(array_value, uint32_t) // Start from 1. 0 is 'None'
#define M_OPL_array_value_t() ARRAY_OPLIST(array_value)

// Define an array of integer to store the seed and register it.
ARRAY_DEF(array_seed, int32_t)
#define M_OPL_array_seed_t() ARRAY_OPLIST(array_seed)
// Define algorithms on array_seed
ALGO_DEF(array_seed, array_seed_t)

// Define an array of constant string that are ordered by their size.
ARRAY_DEF(array_cstr, const char *, M_CSTR_OPLIST)
// Register it globally but with a custom CMP operator
static inline int array_cstr_cmp(const array_cstr_t a, const array_cstr_t b) {
  // We compare only the size
  size_t sa = array_cstr_size(a);
  size_t sb = array_cstr_size(b);
  return (sa > sb) ? -1 : (sa < sb);
}
#define M_OPL_array_cstr_t() M_OPEXTEND(ARRAY_OPLIST(array_cstr, M_CSTR_OPLIST), CMP(array_cstr_cmp))

// Define an array of array of constant string, and some algo over this container
ARRAY_DEF(array_bucket, array_cstr_t)
#define M_OPL_array_bucket_t() ARRAY_OPLIST(array_bucket, M_OPL_array_cstr_t() )
ALGO_DEF(array_bucket, array_bucket_t)

// Define an array of string to save the input.
ARRAY_DEF(array_string, string_t)
#define M_OPL_array_string_t() ARRAY_OPLIST(array_string, M_OPL_string_t())




/* The algorithm may not converge on all inputs if the hash function
 * doesn't randomly distribute the inputs:
 * str1 != str2 ==> \exist seed \such_as hash(seed,str1) != hash(seed, str2)
 * The first hash function below doesn't provide the needed property for
 * inputs with two words 'a' and 'c'
 * The second one is more robust but may still have this issue.
 */
static uint32_t hash(int32_t seed, const char str[])
{
#if 0
  if (seed == 0) seed = 0x01000193;
  while (*str) {
    seed = ((seed * 0x01000193) ^ *str);
    str++;
  }
  return seed;
#else
  if (seed == 0) seed = 0x811C9DC5;
  while (*str) {
    seed = (seed ^ *str) * 16777619;
    str++;
  }
  return seed ^ (seed >> 16);
#endif
}

// Step 1: place all key keys into buckets
static void
CreateMinimalPerfectHash1(array_seed_t seed, array_value_t value,
                          array_bucket_t bucket,
                          const dict_mph_t dict)
{
  size_t size = dict_mph_size (dict);
  
  array_value_resize(value, size);
  array_seed_resize(seed, size);
  array_bucket_resize(bucket, size);

  for M_EACH(item, dict, dict_mph_t) {
      size_t i = hash(0, item->key) % size;
      array_cstr_push_back (*array_bucket_get (bucket, i), item->key);
    }
}

// Step 2:   Sort the buckets and process the ones with the most items first.
static void
CreateMinimalPerfectHash2(array_seed_t seed, array_value_t value,
                          array_bucket_t bucket,
                          const dict_mph_t dict)
{
  size_t size = dict_mph_size (dict);
  array_bucket_sort(bucket);
  M_LET(slots, array_seed_t) {
    for M_EACH(b, bucket, array_bucket_t) {
        if (array_cstr_size(*b) <= 1)
          break;
        size_t d = 1;
        /* Repeatedly try different values of d until we find a hash function
           that places all items in the bucket into free slots.
           This may loop forever on some inputs if the hash function is not good
           enought. */
        for(bool cont = true; cont ; ) {
          cont = false;
          array_seed_reset(slots);
          for M_EACH(it, *b, array_cstr_t) {
              size_t s = hash(d, *it) % size;
              if (*array_value_get(value, s) != 0 || array_seed_contain_p(slots, s)){
                d++;
                cont = true;
                break;
              }
              array_seed_push_back (slots, s);
            }
        }
        // Fill in the seed & value with the computed value
        array_seed_set_at(seed, hash(0, *array_cstr_get(*b, 0)) % size, d);
        d = 0;
        for M_EACH(it, *b, array_cstr_t) {
            array_value_set_at(value,
                               *array_seed_get(slots, d),
                               *dict_mph_get(dict, *it));
            d++;
          }
      }
  }
}

/* Step 3: 
 * Only buckets with 1 item remain. Process them more quickly by directly
 * placing them into a free slot. Use a negative value of d to indicate
 * this.
 */
static void
CreateMinimalPerfectHash3(array_seed_t seed, array_value_t value,
                          array_bucket_t bucket,
                          const dict_mph_t dict)
{
  size_t size = dict_mph_size (dict);

  M_LET(freelist, array_seed_t) {
    // Get a list of available slot
    for(size_t i = 0; i < size; i++)
      if (*array_value_get(value, i) == 0)
        array_seed_push_back(freelist, i);
    for M_EACH(b, bucket, array_bucket_t) {
        if (array_cstr_size(*b) != 1)
          continue;
        int32_t slot;
        array_seed_pop_back(&slot, freelist);
        /*We subtract one to ensure it's negative even if the zeroeth slot was
          used. */
        array_seed_set_at (seed, hash(0, *array_cstr_get(*b, 0)) % size,
                           -slot-1);
        array_value_set_at(value, slot, *dict_mph_get(dict, *array_cstr_get(*b, 0)));
      }
  }
}

static void
CreateMinimalPerfectHash(array_seed_t seed, array_value_t value,
                         const dict_mph_t dict)
{
  M_LET(bucket, array_bucket_t) {
    CreateMinimalPerfectHash1(seed, value, bucket, dict);
    CreateMinimalPerfectHash2(seed, value, bucket, dict);
    CreateMinimalPerfectHash3(seed, value, bucket, dict);
  }
}

static void
dict_read_from_file(dict_mph_t dict, array_string_t arr, const char filename[])
{
  FILE *f = fopen(filename, "rt");
  if (!f) {
    fprintf(stderr, "ERROR: Cannot open %s\n", filename);
    exit(2);
  }

  uint32_t line = 1;
  M_LET(str, string_t) {
    while (!feof(f) && !ferror(f)) {
      string_fgets(str, f, STRING_READ_PURE_LINE);
      string_strim(str);
      if (!string_empty_p(str)) {
        // Not clean as items of 'dict' points are linked to items of 'arr'
        // and the containers are not aware of this link.
        array_string_push_back (arr, str);
        dict_mph_set_at(dict, string_get_cstr(*array_string_back(arr)), line);
        line++;
      }
    }
  }
  fclose(f);
}

static uint32_t
perfect_hash_lockup(array_seed_t seed, array_value_t value,
                    const char key[])
{
  int32_t s = *array_seed_get(seed, hash(0, key) % array_seed_size(seed));
  if (s < 0)
    return *array_value_get(value, -s-1);
  else
    return *array_value_get(value, hash(s, key) % array_value_size(value));
}

static void
test(array_seed_t seed, array_value_t value, dict_mph_t dict)
{
  for M_EACH(item, dict, dict_mph_t) {
      const char *key = item->key;
      uint32_t v = perfect_hash_lockup(seed, value, key);
      if (v != item->value) {
        fprintf(stderr, "ERROR for %s: %u VS %u\n", key, v, item->value);
      }
    }
}

int main(int argc, const char *argv[])
{
  // Create arr as array_string_t, dict as dist_mph_t and seed as array_seed_t
  M_LET(arr, array_string_t) M_LET(dict, dict_mph_t) M_LET(seed, array_seed_t)
    // and value as array_value_t
    M_LET(value, array_value_t) {
      const char *filename = (argc > 1) ? argv[1] :"/usr/share/dict/words";
      dict_read_from_file(dict, arr, filename);
      CreateMinimalPerfectHash(seed, value, dict);
      test(seed, value, dict);
  }
  
  return 0;
}
