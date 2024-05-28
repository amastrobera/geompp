#pragma once

#include "constants.hpp"

#include <string>

namespace geompp {

double round_to(double x, int decimal_precision = 0);

int sign(double x, int decimal_precision = DP_THREE);

std::string trim(std::string s);

std::string to_upper(std::string s);


}  // namespace geompp