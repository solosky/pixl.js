/* Based on the example of STC https://github.com/tylov/STC */
#include "m-dict.h"
#include "m-array.h"
#include "m-deque.h"
#include "m-list.h"
#include "m-rbtree.h"
#include "m-algo.h"
#include "m-tuple.h"

// NOTE: After each definition of a container, we register its oplist
// It is not mandatory, but it makes the code easier to write and read.

// Define a basic structure (POD structure) using a tuple to get the default functions
TUPLE_DEF2(Point, (x, float), (y, float))
#define M_OPL_Point_t() TUPLE_OPLIST(Point, M_BASIC_OPLIST, M_BASIC_OPLIST)

// NOTE: array, list, deque don't have a native find function.
// We'll use the algorithm header to create a find function for such containers.

// Define a set of int & register its oplist
DICT_SET_DEF(cset_i, int)
#define M_OPL_cset_i_t() DICT_SET_OPLIST(cset_i, M_BASIC_OPLIST)

// Define an array of Point, register its oplist & include some basic algorithms
ARRAY_DEF(cvec_p, Point_t)
#define M_OPL_cvec_p_t() ARRAY_OPLIST(cvec_p, M_OPL_Point_t())
ALGO_DEF(cvec_p, cvec_p_t)

// Define a deque of int, including some basic algorithms
DEQUE_DEF(cdeq_i, int)
#define M_OPL_cdeq_i_t() DEQUE_OPLIST(cdeq_i, M_BASIC_OPLIST)
ALGO_DEF(cdeq_i, cdeq_i_t)

// Define a backward list, including some basic algorithms
LIST_DEF(clist_i, int)
#define M_OPL_clist_i_t() LIST_OPLIST(clist_i, M_BASIC_OPLIST)
ALGO_DEF(clist_i, clist_i_t)

// Define an unordered map of int to int
DICT_DEF2(csmap_i, int, int)
#define M_OPL_csmap_i_t() DICT_OPLIST(csmap_i, M_BASIC_OPLIST, M_BASIC_OPLIST)

int main(void)
{
  // Define, init & set the different containers.
  M_LET( (set, 10, 20, 30), cset_i_t)
    M_LET( (vec, (10, 1), (20, 2), (30, 3) ), cvec_p_t)
    M_LET( (deq, 10, 20, 30), cdeq_i_t)
    M_LET( (lst, 10, 20, 30), clist_i_t)
    M_LET( (map, (20, 2), (10, 1), (30,3) ), csmap_i_t) {
    
    // add one more element to each container
    cset_i_push(set, 40);
    cvec_p_push_back(vec, (Point_t) {{40, 4}} );
    cdeq_i_push_front(deq, 5);
    clist_i_push_back(lst, 5);
    csmap_i_set_at(map, 40, 4);

    M_PRINT("vec= ", (vec, cvec_p_t));
    
    // Find an element (either native or through the algo header)
    int *i1 = cset_i_get(set, 20);
    cvec_p_it_t i2; cvec_p_find(i2, vec, (Point_t) {{20, 2}});
    cdeq_i_it_t i3; cdeq_i_find(i3, deq, 20);
    clist_i_it_t i4; clist_i_find(i4, lst, 20);
    int *i5 = csmap_i_get(map, 20);
    printf("\nFound: %d, (%g, %g), %d, %d, [%d: %d]\n",
           *i1,
           (*cvec_p_ref(i2))->x, (*cvec_p_ref(i2))->y,
           *cdeq_i_ref(i3),
           *clist_i_ref(i4),
           20, *i5);

    // Erase the elements found (through key or through iterators)
    cset_i_erase(set, *i1);
    cvec_p_remove(vec, i2);
    cdeq_i_remove(deq, i3);
    clist_i_remove(lst, i4);
    csmap_i_erase(map, *i5);

    // Display everything in one call by using the registered print functions of the different types.
    M_PRINT("After erasing elements found:\n",
            "set: ", (set, cset_i_t), "\n",
            "vec: ", (vec, cvec_p_t), "\n",
            "deq: ", (deq, cdeq_i_t), "\n",
            "map: ", (map, csmap_i_t), "\n");
  }
}
