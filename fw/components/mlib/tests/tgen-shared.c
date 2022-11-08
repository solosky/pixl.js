#include "m-shared.h"

SHARED_PTR_DEF(int, int)

int f(shared_int_t p)
{
  *shared_int_ref(p) = 17;
}

