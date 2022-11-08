//
// -- An A* pathfinder inspired by the tutorial at Red Blob Games --
//     https://www.redblobgames.com/pathfinding/a-star/introduction.html
// Code based on:
//     https://github.com/glouw/ctl/blob/master/examples/astar.c

#include <stdio.h>
#include "m-array.h"
#include "m-string.h"
#include "m-tuple.h"
#include "m-prioqueue.h"
#include "m-dict.h"

// Create a tuple of point, but don't use its priority field for comparison.
TUPLE_DEF2( point, (x, int), (y, int), (priority, int, M_OPEXTEND( M_OPL_int(), CMP(0), HASH(0), EQUAL(0) ) ) )
#define M_OPL_point_t() TUPLE_OPLIST(point, M_OPL_int(), M_OPL_int(), M_OPL_int())
static inline int point_cmp_prio(const point_t a, const point_t b)
{
  return a->priority < b->priority ? -1 : a->priority > b->priority;
}
PRIOQUEUE_DEF(prio_point, point_t, M_OPEXTEND(M_OPL_point_t(), CMP(point_cmp_prio) ) )
#define M_OPL_prio_point_t() PRIOQUEUE_OPLIST(prio_point, M_OPL_point_t() )
ARRAY_DEF(array_point, point_t)
#define M_OPL_array_point_t() ARRAY_OPLIST(array_point, M_OPL_point_t() )
DICT_DEF2(cost_point, point_t, int)
#define M_OPL_cost_point_t() DICT_OPLIST(cost_point, M_OPL_point_t(), M_OPL_int())
DICT_DEF2(step_point, point_t, point_t)
#define M_OPL_step_point_t() DICT_OPLIST(step_point, M_OPL_point_t(), M_OPL_point_t())

static inline int point_index(const point_t p, int width)
{
  return p->x + width * p->y;
}

static inline void point_set_from(point_t p, const string_t maze, int width, const char pattern[])
{
  size_t s = string_search_str(maze, pattern);
  point_emplace(p, s % width, s / width , 0);
}

static void astar(array_point_t path, const string_t maze, int width)
{
  M_LET(front, prio_point_t)
    M_LET(from, step_point_t)
    M_LET(costs, cost_point_t)
    M_LET(start, goal, current, next, point_t)
    {
      point_set_from(start, maze, width, "@");
      point_set_from(goal, maze, width, "!");
      cost_point_set_at(costs, start, 0);
      prio_point_push(front, start);
      while (!prio_point_empty_p(front)) {
        prio_point_pop(&current, front);
        if (point_equal_p(current, goal))
          break;
        const struct { int x; int y; } deltas[] = {
          { -1, +1 }, { 0, +1 }, { 1, +1 },
          { -1,  0 }, /* ~ ~ */  { 1,  0 },
          { -1, -1 }, { 0, -1 }, { 1, -1 },
        };
        for (size_t i = 0; i < 8; i++) {
          point_emplace(next, current->x + deltas[i].x, current->y + deltas[i].y, 0);
          if (string_get_char(maze, point_index(next, width)) != '#') {
            int *new_cost = cost_point_get(costs, current);
            int *cost     = cost_point_get(costs, next);
            if (cost == NULL || *new_cost < *cost) {
              cost_point_set_at(costs, next, *new_cost);
              next->priority = *new_cost + abs(goal->x - next->x) + abs(goal->y - next->y);
              prio_point_push(front, next);
              step_point_set_at(from, next, current);
            }
          }
        }
      }     
      point_set(current, goal);
      while (!point_equal_p(current, start)) {
        array_point_push_back(path, current);
        point_set(current, *step_point_get(from, current));
      }
      array_point_push_back(path, start);
    }
}

int main(void)
{
  M_LET( (maze, 
          ("#########################################################################\n"
           "#   #               #               #           #                   #   #\n"
           "#   #   #########   #   #####   #########   #####   #####   #####   # ! #\n"
           "#               #       #   #           #           #   #   #       #   #\n"
           "#########   #   #########   #########   #####   #   #   #   #########   #\n"
           "#       #   #               #           #   #   #   #   #           #   #\n"
           "#   #   #############   #   #   #########   #####   #   #########   #   #\n"
           "#   #               #   #   #       #           #           #       #   #\n"
           "#   #############   #####   #####   #   #####   #########   #   #####   #\n"
           "#           #       #   #       #   #       #           #   #           #\n"
           "#   #####   #####   #   #####   #   #########   #   #   #   #############\n"
           "#       #       #   #   #       #       #       #   #   #       #       #\n"
           "#############   #   #   #   #########   #   #####   #   #####   #####   #\n"
           "#           #   #           #       #   #       #   #       #           #\n"
           "#   #####   #   #########   #####   #   #####   #####   #############   #\n"
           "#   #       #           #           #       #   #   #               #   #\n"
           "#   #   #########   #   #####   #########   #   #   #############   #   #\n"
           "#   #           #   #   #   #   #           #               #   #       #\n"
           "#   #########   #   #   #   #####   #########   #########   #   #########\n"
           "#   #       #   #   #           #           #   #       #               #\n"
           "# @ #   #####   #####   #####   #########   #####   #   #########   #   #\n"
           "#   #                   #           #               #               #   #\n"
           "#########################################################################\n")), string_t)
    M_LET(path, array_point_t)
    {
      int width = string_search_str(maze, "\n") + 1;
      astar(path, maze, width);
      for M_EACH(it, path, array_point_t) {
          string_set_char(maze, point_index(*it, width), 'x');
        }
      printf("%s", string_get_cstr(maze));
    }
}
