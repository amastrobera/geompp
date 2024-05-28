#pragma once

#include "constants.hpp"

#include <string>
#include <vector>

namespace geompp {

double round_to(double x, int decimal_precision = 0);

int sign(double x, int decimal_precision = DP_THREE);

std::string trim(std::string s);

std::string to_upper(std::string s);

std::vector<double> tokenize_space_separated_string_to_doubles(std::string const& str);

}  // namespace geompp