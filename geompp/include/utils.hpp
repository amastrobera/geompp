#pragma once

#include <cmath>
#include <sstream>

namespace geompp {

  inline double round_to(double x, int decimal_precision = 0) {
    double exp = pow(10, decimal_precision);
    return round(x * exp) / exp;
  }



}