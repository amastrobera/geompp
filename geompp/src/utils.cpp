#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>  // debug only
#include <ranges>
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

  std::cout << "str=" << str << std::endl;

  if (delimiter == ' ') {
    double dtoken;
    iss.precision(10);
    while (iss >> dtoken) {
      std::cout << "dtoken=" << dtoken << std::endl;
      tokens.push_back(dtoken);
    }

  } else {
    std::string stoken;
    while (std::getline(iss, stoken, delimiter)) {
      try {
        tokens.push_back(std::stod(stoken));

      } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid token encountered: " + stoken);
      } catch (const std::out_of_range& e) {
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
  size_t decimal_pos = number_str.find('.');
  if (decimal_pos == std::string::npos) {
    return 0;
  }
  // Remove trailing zeros after the decimal point
  size_t last_non_zero = number_str.find_last_not_of('0');
  if (last_non_zero != std::string::npos && last_non_zero > decimal_pos) {
    return static_cast<int>(last_non_zero - decimal_pos);
  }
  return 0;
}

}  // namespace geompp