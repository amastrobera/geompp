#include "utils.hpp"

#include <cmath>
#include <sstream>

namespace geompp {

double round_to(double x, int decimal_precision) {
  double exp = pow(10, decimal_precision);
  return round(x * exp) / exp;
}

int sign(double x, int decimal_precision) {
  if (round_to(x, decimal_precision) >= 0) {
    return 1;
  }
  return -1;
}

}  // namespace geompp