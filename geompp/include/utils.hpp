#pragma once

#include "constants.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace geompp {

double round_to(double x, int decimal_precision = 0);

int sign(double x, int decimal_precision = DP_THREE);

std::string trim(std::string s);

std::string to_upper(std::string s);

std::vector<double> tokenize_to_doubles(std::string const& str, char delimiter = ' ');

std::vector<std::string> tokenize_string(std::string const& str, char delimiter = ',');

template <typename T>
std::string string_join(std::vector<T> const& items, std::string const& delim = " ") {
  std::ostringstream buf;
  for (int i = 0; i < items.size(); ++i) {
    buf << items[i];
    if (i < items.size() - 1) {
      buf << delim;
    }
  }
  return buf.str();
}

int count_decimal_places(double number);

}  // namespace geompp