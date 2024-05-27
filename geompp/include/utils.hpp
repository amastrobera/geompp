#pragma once

#include "constants.hpp"

namespace geompp {

double round_to(double x, int decimal_precision = 0);

int sign(double x, int decimal_precision = DP_THREE);

}  // namespace geompp