#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <compare>
#include <iomanip>   // For setting precision in debug output
#include <iostream>  // debug only
#include <ranges>
#include <sstream>

namespace geompp {

double round(double x, int decimal_precision) {
  double exp = pow(10, decimal_precision);
  return std::round(x * exp) / exp;
}

std::partial_ordering compare(double a, double b, double epsilon) {
  if (std::abs(a - b) <= epsilon) {
    return std::partial_ordering::equivalent;
  }
  return a <=> b;  // starship operator gracefull handles the NaN case
}

bool is_in_range(double value, double min, double max, double epsilon) {
  return compare(value, min, epsilon) >= 0 && compare(value, max, epsilon) <= 0;
}

bool is_greater_or_equal(double value, double threshold, double epsilon) {
  return compare(value, threshold, epsilon) >= 0;
}

int sign(double x) {
  if (compare(x, 0) >= 0) {
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

std::vector<double> tokenize_to_doubles(std::string const& str, char delimiter) {
  std::vector<double> tokens;
  std::istringstream iss(str);
  std::string stoken;

  if (delimiter == ' ') {
    while (iss >> stoken) {
      tokens.push_back(std::stod(stoken));
      iss.precision(10);
    }

  } else {
    while (std::getline(iss, stoken, delimiter)) {
      try {
        tokens.push_back(std::stod(stoken));

      } catch (const std::invalid_argument) {
        throw std::runtime_error("Invalid token encountered: " + stoken);

      } catch (const std::out_of_range) {
        throw std::runtime_error("Token out of range: " + stoken);
      }
    }
  }

  return tokens;
}

std::vector<std::string> tokenize_string(std::string const& str, char delimiter) {
  auto split_view = str | std::views::split(delimiter);
  std::vector<std::string> tokens;

  for (const auto& part : split_view) {
    tokens.emplace_back(part.begin(), part.end());
  }

  return tokens;
}

int count_decimal_places(double number) {
  std::string number_str = std::to_string(number);
  std::size_t decimal_pos = number_str.find('.');
  if (decimal_pos == std::string::npos) {
    return 0;
  }
  // Remove trailing zeros after the decimal point
  std::size_t last_non_zero = number_str.find_last_not_of('0');
  if (last_non_zero != std::string::npos && last_non_zero > decimal_pos) {
    return static_cast<int>(last_non_zero - decimal_pos);
  }
  return 0;
}

void remove_duplicates(std::vector<double>& sorted_vec, double epsilon) {
  if (sorted_vec.empty()) {
    return;
  }
  // Filter out consecutive elements that evaluate to 'equivalent'
  auto [first, last] = std::ranges::unique(sorted_vec, [epsilon](double a, double b) {
    return compare(a, b, epsilon) == std::partial_ordering::equivalent;
  });

  // Physically shrink the vector container
  sorted_vec.erase(first, last);
}

void remove_all_duplicated_elements(std::vector<double>& sorted_vec, double epsilon) {
  if (sorted_vec.empty()) {
    return;
  }

  auto write_it = sorted_vec.begin();

  for (auto read_it = sorted_vec.begin(); read_it != sorted_vec.end();) {
    auto next_it = read_it;

    // Advance next_it to find the end of the current matching block
    while (next_it != sorted_vec.end() && std::abs(*next_it - *read_it) <= epsilon) {
      ++next_it;
    }

    // If the block size is EXACTLY 1, this element is truly unique!
    if (next_it - read_it == 1) {
      *write_it = *read_it;  // Retain the element
      ++write_it;
    }

    // Skip the entire duplicated block
    read_it = next_it;
  }

  // Physically chop off the dead space at the back
  sorted_vec.erase(write_it, sorted_vec.end());
}

}  // namespace geompp
