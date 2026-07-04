#pragma once

#include "constants.hpp"

#include <compare>
#include <concepts>
#include <sstream>
#include <string>
#include <vector>

namespace geompp {

double round(double x, int decimal_precision = DECIMAL_PRECISION);

std::partial_ordering compare(double a, double b, double epsilon = DOUBLE_EPSILON);

bool is_in_range(double value, double min, double max, double epsilon = DOUBLE_EPSILON);

bool is_greater_or_equal(double value, double threshold, double epsilon = DOUBLE_EPSILON);

int sign(double x);

std::string trim(std::string s);

std::string to_upper(std::string s);

std::vector<double> tokenize_to_doubles(std::string const& str, char delimiter = ' ');

std::vector<std::string> tokenize_string(std::string const& str, char delimiter = ',');

int count_decimal_places(double number);

/// @brief removes duplicates from a sorted vector (duplicates are consecutive)
///        input {0,0,0,1,2,3,4,4,5} --> output: {0,1,2,3,4,5}
/// @param sorted_vec vector of sorted elements
void remove_duplicates(std::vector<double>& sorted_vec, double epsilon = DOUBLE_EPSILON);

/// @brief removes ALL elements that have (consecutve) duplicates and the elements themselves.
///        input {0,0,0,1,2,3,4,4,5} --> output: {1,2,3,5}
/// @param sorted_vec vector of sorted elements
void remove_all_duplicated_elements(std::vector<double>& sorted_vec, double epsilon = DOUBLE_EPSILON);

}  // namespace geompp

#pragma region Template Implementation

namespace geompp {

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

template <typename T>
requires requires(T t) {
  { t.ToWkt() }
  ->std::convertible_to<std::string>;
}
std::string ToWkt(const std::vector<T>& items) {
  std::string out = "GEOMETRYCOLLECTION(";
  for (std::size_t i = 0; i < items.size(); ++i) {
    if (i > 0) {
      out += ", ";
    }
    out += items[i].ToWkt();
  }
  return out + ")";
}

}  // namespace geompp

#pragma endregion
