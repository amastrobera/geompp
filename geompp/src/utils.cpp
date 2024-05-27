#include "utils.hpp"

#include <cmath>
#include <sstream>

namespace geompp {

double round_to(double x, int decimal_precision) {
  double exp = pow(10, decimal_precision);
  return round(x * exp) / exp;
}

}  // namespace geompp