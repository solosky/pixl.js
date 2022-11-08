#include <stdio.h>

#include "m-tuple.h"
#include "m-array.h"
#include "m-string.h"
#include "m-list.h"
#include "m-buffer.h"

/* This example chains the structure in different ways in
   order to show/test the good chaining of the structures */

TUPLE_DEF2(symbol, (type, int), (name, string_t))
#define M_OPL_symbol_t() TUPLE_OPLIST(symbol, M_BASIC_OPLIST, STRING_OPLIST)

ARRAY_DEF(symbol_list, symbol_t)
#define M_OPL_symbol_list_t() ARRAY_OPLIST(symbol_list, M_OPL_symbol_t())

TUPLE_DEF2(record, (fields, symbol_list_t))
#define M_OPL_record_t() TUPLE_OPLIST(record, M_OPL_symbol_list_t())

TUPLE_DEF2(my_tuple, (name, string_t), (value, int))
#define M_OPL_my_tuple_t() TUPLE_OPLIST(my_tuple, STRING_OPLIST, M_BASIC_OPLIST)

ARRAY_DEF(my_list_of_tuple, my_tuple_t)
#define M_OPL_my_list_of_tuple_t() ARRAY_OPLIST(my_list_of_tuple)

TUPLE_DEF2(my_tuple_of_tuple, (name, string_t), (inner_tuple, my_tuple_t))
#define M_OPL_my_tuple_of_tuple_t() TUPLE_OPLIST(my_tuple_of_tuple, STRING_OPLIST, M_OPL_my_tuple_t() )

LIST_DEF(list_of_tuple_of_tuple, my_tuple_of_tuple_t)
#define M_OPL_list_of_tuple_of_tuple_t() LIST_OPLIST(list_of_tuple_of_tuple, M_OPL_my_tuple_of_tuple_t() )

TUPLE_DEF2(my_triple_tuple, (name, string_t), (inner_tuple, my_tuple_t), (list, list_of_tuple_of_tuple_t) )
#define M_OPL_my_triple_tuple_t() TUPLE_OPLIST(my_triple_tuple, STRING_OPLIST, M_OPL_my_tuple_t(), M_OPL_list_of_tuple_of_tuple_t())

BUFFER_DEF(trbuff, my_triple_tuple_t, 100, BUFFER_QUEUE)
#define M_OPL_trbuff_t() BUFFER_OPLIST(trbuff, M_OPL_my_triple_tuple_t())

TUPLE_DEF2(my_junction, (active, bool), (buffer, trbuff_t))
#define M_OPL_my_junction_t() TUPLE_OPLIST(my_junction, M_BASIC_OPLIST, M_OPL_trbuff_t())
           
TUPLE_DEF2(mlib_point, (x, int), (y, int))
#define M_OPL_mlib_point_t()                                            \
  TUPLE_OPLIST(mlib_point, M_BASIC_OPLIST, M_BASIC_OPLIST)

TUPLE_DEF2(mlib_points, (first_ref, mlib_point_ptr, M_PTR_OPLIST),
           (second_ref, mlib_point_ptr, M_PTR_OPLIST),
           (third, mlib_point_t))
#define M_OPL_mlib_points_t()                                           \
  TUPLE_OPLIST(mlib_points, M_PTR_OPLIST, M_PTR_OPLIST, M_OPL_mlib_point_t())

static void let2(void)
{
  // Initialize p1 as mlib_point_t with (4,4)
  // and p2 as mlib_point_t with (5,5)
  // and pair as mlib_point_t with default value
  M_LET((p1, 4, 4), (p2, 5, 5), mlib_point_t) {
    M_LET(pair, mlib_points_t) {
      pair->first_ref = p1;
      pair->second_ref = p2;
      pair->third->x = pair->third->y = 6;
      
      assert(pair->first_ref->x == 4);
      p1->x = 10;
      assert(pair->first_ref->x == 10);
      
      assert(pair->third->x == 6);
    }
  }
}

static void let(void)
{
  // Let's initialize all the declared types
  M_LET(s, symbol_t)
    M_LET(l, symbol_list_t)
    M_LET(r,record_t)
    M_LET(tup, my_tuple_t)
    M_LET(l2, my_list_of_tuple_t)
    M_LET(tuptup, my_tuple_of_tuple_t)
    M_LET(ltuptup, list_of_tuple_of_tuple_t)
    M_LET(tuptuptup, my_triple_tuple_t)
    M_LET(junc, my_junction_t)
    {
      printf("Init ok\n");
    }
}

int main(void)
{
  let();
  let2();
}

