#include <math.h>

#include "m-array.h"
ARRAY_DEF(double, double)

int main(void) {
  const int n = 250000000;
  const double x = 0.001;
  M_LET(v, ARRAY_OPLIST(double)) {
      array_double_resize(v, n);
#pragma omp parallel for
      for (int i = 0; i < array_double_size(v); ++i) {
        array_double_set_at(v, i, cos(i * x));
      }
  }

  return 0;
}
