#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ranges>
#include <sstream>
#include <string>


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

std::string trim(std::string s) {
  auto not_space = [](unsigned char c) { return !std::isspace(c); };

  // erase the the spaces at the back first
  // so we don't have to do extra work
  s.erase(std::ranges::find_if(s | std::views::reverse, not_space).base(), s.end());

  // erase the spaces at the front
  s.erase(s.begin(), std::ranges::find_if(s, not_space));

  return s;
}

std::string to_upper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

}  // namespace geompp